#include <H3D/RenderMetaData.h>

using namespace H3D;

/************************************************************************/
/*                       ElementDrawData definitions                  */
/************************************************************************/

H3D::ElementDrawData::ElementDrawData()
: IBO(0), indexCount(0), VBOs(0){
}

/************************************************************************/
/*                     RenderData definitions                           */
/************************************************************************/

H3D::RenderData::RenderData() : renderState(0), vertexAttributeLayout(0)
{
}

/************************************************************************/
/*                      Renderbucket definitions                        */
/************************************************************************/

H3D::RenderBucket::RenderBucket() : renderables(0), elementRenderables(0), renderState(0), vertexAttributeLayout(0), numVBOs(0)
{
}
