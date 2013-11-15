//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2013 Michael Fink
//
//! \file SkyRenderManager.cpp Sky render manager
//

// includes
#include "StdAfx.h"
#include "SkyRenderManager.hpp"
#include "IFileSystem.hpp"
#include "TextureLoader.hpp"
#include "OpenGL.hpp"
#include "TextureParameter.hpp"
#include "AstronomyMath.hpp"
#include "BillboardMatrix.hpp"
#include <ulib/DateTime.hpp>
#include "RenderOptions.hpp"

/// skydome radius
const double c_dSkydomeRadius = 1000.0;

/// sun diameter/distance: 0.00936 = 0.53623 degrees as seen from earth
/// \see http://vterrain.org/Atmosphere/
const double c_dSunDiameterAngle = 0.53623;

/// moon diameter/distance: 0.00904 = 0.51791 degrees as seen from earth
const double c_dMoonDiameterAngle = 0.51791;

/// local latitude
const double c_dLocalLatitude = 48.148221;

/// local longitude
const double c_dLocalLongitude = 11.57959;


SkyRenderManager::SkyRenderManager(GraphicsTaskManager& taskManager, IFileSystem& fileSystem)
:m_dAngleDirection(0.0),
 m_dAngleUp(0.0),
 m_dFov(90.0),
 m_dRatio(1.333),
 m_milkyWaySkyboxRenderer(taskManager, fileSystem),
 m_spTextureLoader(new TextureLoader(taskManager, fileSystem))
{
   m_dtNow = DateTime::Now();
}

void SkyRenderManager::Prepare()
{
   if (m_milkyWaySkyboxRenderer.IsPrepareNeeded())
      m_milkyWaySkyboxRenderer.Prepare();

   m_spTexSun.reset(new Texture);
   m_spTexMoon.reset(new Texture);
   m_spTexSkyTonesMap.reset(new Texture);

   //m_spTextureLoader->Load(_T("sky\\sun.png"), m_spTexSun, false);
   //m_spTextureLoader->Load(_T("sky\\fm1222_gendler_big_alpha.png"), m_spTexMoon, false);
   //m_spTextureLoader->Load(_T("sky\\clearSky2_norle.png"), m_spTexSkyTonesMap, false);
   m_spTextureLoader->Load(_T("sky\\sun.tga"), m_spTexSun, false);
   m_spTextureLoader->Load(_T("sky\\fm1222_gendler_big_alpha.tga"), m_spTexMoon, false);
   m_spTextureLoader->Load(_T("sky\\clearSky2_norle.tga"), m_spTexSkyTonesMap, false);
}

void SkyRenderManager::Upload()
{
   if (m_milkyWaySkyboxRenderer.IsUploadNeeded())
      m_milkyWaySkyboxRenderer.Upload();
}

void SkyRenderManager::Render(RenderOptions& options)
{
   glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

   glDisable(GL_FOG);
   glDisable(GL_LIGHTING);
   glDisable(GL_DEPTH_TEST);

   glPushMatrix();
   glLoadIdentity();

   // render night sky dome
   RenderNightSky(options);

   // render day skydome (with clouds)
   RenderDaySky();

   // draw sun, moon
   RenderSun();
   RenderMoon();

   // for help, draw lines
   if (options.Get(RenderOptions::optionSkyMeshLines))
      RenderLines();

   glPopMatrix();

   glPopAttrib();
}

void SkyRenderManager::Done()
{
   m_spTexSkyTonesMap.reset();
   m_spTexSun.reset();
   m_spTexMoon.reset();
}

Vector3d SkyRenderManager::PosFromAngles(double dRadius, double dElevation, double dAzimuth)
{
   Vector3d vDir(dRadius, 0.0, 0.0);
   vDir.RotateZ(dElevation);
   vDir.RotateY(dAzimuth);
   return vDir;
}

void SkyRenderManager::SunDirection(double& dElevation, double& dAzimuth)
{
   AstronomyMath::SunPosition(m_dtNow,
      c_dLocalLatitude, c_dLocalLongitude,
      dAzimuth, dElevation);
}

void SkyRenderManager::MoonDirection(double& dElevation, double& dAzimuth)
{
   AstronomyMath::MoonPosition(m_dtNow,
      c_dLocalLatitude, c_dLocalLongitude,
      dAzimuth, dElevation);
}

void SkyRenderManager::RenderLines()
{
   glDisable(GL_TEXTURE_2D);

   glColor4ub(255, 255, 255, 0);

   glBegin(GL_LINES);

   Vector3d vDir;
   for (unsigned int uAzimuth = 0; uAzimuth <= 360; uAzimuth += 10)
   for (unsigned int uElevation = 0; uElevation < 90; uElevation += 10)
   {
      for (unsigned int i=0; i<4; i++)
      {
         vDir = PosFromAngles(c_dSkydomeRadius,
            uElevation + (i == 3 ? 10 : 0),
            uAzimuth + (i == 1 ? 10 : 0));

         glVertex3dv(vDir.Data());
      }
   }

   for (unsigned int uAzimuth = 0; uAzimuth <= 360; uAzimuth += 10)
   {
      // p1
      vDir = PosFromAngles(c_dSkydomeRadius, 0.0, uAzimuth);
      glVertex3dv(vDir.Data());

      // p2
      vDir -= Vector3d(0.0, c_dSkydomeRadius, 0.0);
      glVertex3dv(vDir.Data());

      // p3
      glVertex3dv(vDir.Data());

      // p4
      vDir = PosFromAngles(c_dSkydomeRadius, 0.0, uAzimuth + 10);
      vDir -= Vector3d(0.0, c_dSkydomeRadius, 0.0);
      glVertex3dv(vDir.Data());
   }
   glEnd();

   // render sun position
   {
      double dElevation, dAzimuth;
      SunDirection(dElevation, dAzimuth);

      double dRad = 10.0 * c_dSunDiameterAngle / 2.0;

      glBegin(GL_LINE_LOOP);
      {
         vDir = PosFromAngles(c_dSkydomeRadius, dElevation-dRad, dAzimuth-dRad);
         glVertex3dv(vDir.Data());

         vDir = PosFromAngles(c_dSkydomeRadius, dElevation-dRad, dAzimuth+dRad);
         glVertex3dv(vDir.Data());

         vDir = PosFromAngles(c_dSkydomeRadius, dElevation+dRad, dAzimuth+dRad);
         glVertex3dv(vDir.Data());

         vDir = PosFromAngles(c_dSkydomeRadius, dElevation+dRad, dAzimuth-dRad);
         glVertex3dv(vDir.Data());
      }
      glEnd();
   }

   // render moon position
   {
      double dElevation, dAzimuth;
      MoonDirection(dElevation, dAzimuth);

      double dRad = 10.0 * c_dMoonDiameterAngle / 2.0;

      glBegin(GL_LINE_LOOP);
      {
         vDir = PosFromAngles(c_dSkydomeRadius, dElevation+dRad, dAzimuth);
         glVertex3dv(vDir.Data());

         vDir = PosFromAngles(c_dSkydomeRadius, dElevation,      dAzimuth+dRad);
         glVertex3dv(vDir.Data());

         vDir = PosFromAngles(c_dSkydomeRadius, dElevation-dRad, dAzimuth);
         glVertex3dv(vDir.Data());

         vDir = PosFromAngles(c_dSkydomeRadius, dElevation,      dAzimuth-dRad);
         glVertex3dv(vDir.Data());
      }
      glEnd();
   }
}

void SkyRenderManager::RenderNightSky(RenderOptions& options)
{
   if (m_dtNow.Hour() > 5 && m_dtNow.Hour() < 19)
      return; // don't render sky at daytime

   m_milkyWaySkyboxRenderer.Render(options);
}

typedef TextureParameter<GL_REPEAT, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, GL_MODULATE> SkyTexParameter;

void SkyRenderManager::RenderDaySky()
{
   glColor4ub(255, 255, 255, 255);
   m_spTexSkyTonesMap->Bind();

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   SkyTexParameter::Use();

   // the x tex coord
   unsigned int uiDaytimeMinutes = m_dtNow.Hour() * 60 + m_dtNow.Minute();
   const float s = uiDaytimeMinutes / (60*24.f) + 0.75f + 0.04f;

   unsigned int uiQuads = 0;

   // render sky dome
   Vector3d vDir;
   glBegin(GL_QUADS);
   for (unsigned int uAzimuth = 0; uAzimuth <= 360; uAzimuth += 5)
   for (unsigned int uElevation = 0; uElevation < 90; uElevation += 5)
   {
      // the y tex coords
      float t1 = uElevation / 90.0f;
      float t2 = (uElevation+5) / 90.0f;

      glTexCoord2f(s, t1);
      vDir = PosFromAngles(c_dSkydomeRadius, uElevation, uAzimuth);
      glVertex3dv(vDir.Data());

      glTexCoord2f(s, t2);
      vDir = PosFromAngles(c_dSkydomeRadius, uElevation+5, uAzimuth);
      glVertex3dv(vDir.Data());

      glTexCoord2f(s, t2);
      vDir = PosFromAngles(c_dSkydomeRadius, uElevation+5, uAzimuth+5);
      glVertex3dv(vDir.Data());

      glTexCoord2f(s, t1);
      vDir = PosFromAngles(c_dSkydomeRadius, uElevation, uAzimuth+5);
      glVertex3dv(vDir.Data());

      uiQuads++;
   }

   // render bottom cylinder
   for (unsigned int uAzimuth = 0; uAzimuth <= 360; uAzimuth += 5)
   {
      float t = 0.0;
      glTexCoord2f(s, t);

      // p1
      glColor4ub(255, 255, 255, 255);
      vDir = PosFromAngles(c_dSkydomeRadius, 0.0, uAzimuth);
      glVertex3dv(vDir.Data());

      // p2
      vDir = PosFromAngles(c_dSkydomeRadius, 0.0, uAzimuth+5);
      glVertex3dv(vDir.Data());

      // p3
      vDir -= Vector3d(0.0, c_dSkydomeRadius, 0.0);
      glColor4ub(0, 0, 0, 255);
      glVertex3dv(vDir.Data());

      // p4
      vDir = PosFromAngles(c_dSkydomeRadius, 0.0, uAzimuth);
      vDir -= Vector3d(0.0, c_dSkydomeRadius, 0.0);
      glVertex3dv(vDir.Data());

      uiQuads++;
   }
   glEnd();

   glDisable(GL_BLEND);

   OpenGL::CountPolygons(uiQuads*2);
}

void SkyRenderManager::RenderSun()
{
   m_spTexSun->Bind();

   double dElevation, dAzimuth;
   SunDirection(dElevation, dAzimuth);

   double dRad = 400.0 * c_dSunDiameterAngle;

   PointBillboardMatrix billboard(PosFromAngles(c_dSkydomeRadius, dElevation, dAzimuth));

   Vector3d vCameraPos, vCameraUp;
   BillboardMatrixBase::GetCameraPosAndUp(vCameraPos, vCameraUp);

   billboard.Update(vCameraPos, vCameraUp);

   glColor4ub(255, 255, 128, 208);
   glEnable(GL_TEXTURE_2D);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glPushMatrix();
   billboard.ApplyMatrix();

   glBegin(GL_QUADS);
      glTexCoord2d(0.0, 0.0); glVertex3d(-dRad, -dRad, 0.0);
      glTexCoord2d(0.0, 1.0); glVertex3d(+dRad, -dRad, 0.0);
      glTexCoord2d(1.0, 1.0); glVertex3d(+dRad, +dRad, 0.0);
      glTexCoord2d(1.0, 0.0); glVertex3d(-dRad, +dRad, 0.0);
   glEnd();

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

void SkyRenderManager::RenderMoon()
{
   m_spTexMoon->Bind();

   double dElevation, dAzimuth;
   MoonDirection(dElevation, dAzimuth);

   double dRad = 150.0 * c_dMoonDiameterAngle;

   PointBillboardMatrix billboard(PosFromAngles(c_dSkydomeRadius, dElevation, dAzimuth));

   Vector3d vCameraPos, vCameraUp;
   BillboardMatrixBase::GetCameraPosAndUp(vCameraPos, vCameraUp);

   billboard.Update(vCameraPos, vCameraUp);

   glColor4ub(255, 255, 255, 208);
   glEnable(GL_TEXTURE_2D);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glPushMatrix();
   billboard.ApplyMatrix();

   glBegin(GL_QUADS);
      glTexCoord2d(0.0, 0.0); glVertex3d(-dRad, -dRad, 0.0);
      glTexCoord2d(0.0, 1.0); glVertex3d(+dRad, -dRad, 0.0);
      glTexCoord2d(1.0, 1.0); glVertex3d(+dRad, +dRad, 0.0);
      glTexCoord2d(1.0, 0.0); glVertex3d(-dRad, +dRad, 0.0);
   glEnd();

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}
