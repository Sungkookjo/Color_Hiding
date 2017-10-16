//////////////////////////////////////////////////////////////////////////
// PCH
#pragma once

#ifdef	WIN32
#define NOMINMAX	// for physx
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#endif

#include <Windows.h>
#include <cmath>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dsound.h>
#include <Dshow.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
//#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <time.h>
#include <iostream>
#include <process.h>
using namespace std;

#ifdef UNICODE
#define tstring wstring
#else
#define tstring	string
#endif

// 피직스
#include "NxPhysics.h"
#include "PhysXSDK.h"
#include "NxCooking.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "Nxcontrollermanager.h"
#include "PhysXLoader.h"
#include "NxStream.h"
#include "Stream.h"
//압축
#include "zlib.h"
#include "basefw_fs.h"

//////////////////////////////////////////////////////////////////////////
// lib
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
// 피직스
#pragma comment(lib,"PhysXLoader.lib")
#pragma comment(lib,"NxCooking.lib")
// 인풋
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
// 사운드
#pragma comment(lib,"dsound.lib")
// 다엑쇼
#pragma comment(lib,"Strmiids.lib")
// 압축
#pragma comment(lib,"zdll.lib")

#include "Common.h"
#include "Utility.h"
#include "DShowUlt.h"
#include "Timer.h"
#include "Frustrum.h"
#include "basefw_fs.h"

#include "JDevice.h"
#include "O2Actor.h"
#include "O2Input.h"
#include "O2Camera.h"
#include "O2ParticleSystem.h"
#include "O2SoundUtil.h"
#include "Tsound.h"
#include "O2AI.h"

#include "Hierarchy.h"
#include "O2Texture.h"
#include "O2Mesh.h"
#include "O2ActiveMesh.h"
#include "ResourceManager.h"

#include "Effect.h"

#include "O2GUIObject.h"

#include "O2Object.h"
//#include "O22DObject.h"
//#include "O23DObject.h"
//#include "O2PassiveObject.h"
//#include "O2ActiveObject.h"