#include <CppUtil/Engine/WireframeRaster.h>

#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Engine/Gooch.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/Shader.h>

#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

using namespace CppUtil::Engine;
using namespace CppUtil::QT;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace std;

void WireframeRaster::Init() {
	ForwardRaster::Init();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	SetDrawSky(false);
}
