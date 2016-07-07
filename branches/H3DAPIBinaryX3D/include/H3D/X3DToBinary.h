#ifndef __X3DToBinary_H__
#define __X3DToBinary_H__


#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

#include <fstream>

#include <sstream>

#include <string>

#include <vector>
#include <iterator>
#include <map>

#include <tinyxml.h>

#include <iostream>

#include <algorithm>

#include <list>

#include <H3D/H3DNodeDatabase.h>
#include <H3D/GlobalSettings.h>

#include <H3D/ResourceResolver.h>
#include <H3D/H3DExports.h>
#include <H3DUtil/Exception.h>
#include <H3D/X3DFieldConversion.h>
#include <H3D/X3DTypeFunctions.h>
#include <H3D/Inline.h>

#include <H3D/Node.h>
#include <H3D/Field.h>
#include <H3D/H3DScriptNode.h>

#include <H3D/SFNode.h>
#include <H3D/MFNode.h>

#include <H3D/DEFNodes.h>

#include <H3D/X3DTypes.h>

#include <H3D/SField.h>



#include <H3D/SFFloat.h>
#include <H3D/SFDouble.h>
#include <H3D/SFTime.h>
#include <H3D/SFInt32.h>
#include <H3D/SFVec2f.h>
#include <H3D/SFVec2d.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFVec3d.h>
#include <H3D/SFVec4f.h>
#include <H3D/SFVec4d.h>

#include <H3D/SFBool.h>
#include <H3D/SFString.h>

#include <H3D/SFNode.h>

#include <H3D/SFColor.h>
#include <H3D/SFColorRGBA.h>
#include <H3D/SFRotation.h>
#include <H3D/SFQuaternion.h>

#include <H3D/SFMatrix3f.h>
#include <H3D/SFMatrix4f.h>
#include <H3D/SFMatrix3d.h>
#include <H3D/SFMatrix4d.h>


//TODO:Rustam: what to do with images?!
//#include <H3D/SFImage>

#include <H3D/MFFloat.h>
#include <H3D/MFDouble.h>
#include <H3D/MFTime.h>
#include <H3D/MFInt32.h>
#include <H3D/MFVec2f.h>
#include <H3D/MFVec2d.h>
#include <H3D/MFVec3f.h>
#include <H3D/MFVec3d.h>
#include <H3D/MFVec4f.h>
#include <H3D/MFVec4d.h>

#include <H3D/MFBool.h>
#include <H3D/MFString.h>

#include <H3D/MFNode.h>

#include <H3D/MFColor.h>
#include <H3D/MFColorRGBA.h>
#include <H3D/MFRotation.h>
#include <H3D/MFQuaternion.h>

#include <H3D/MFMatrix3f.h>
#include <H3D/MFMatrix4f.h>
#include <H3D/MFMatrix3d.h>
#include <H3D/MFMatrix4d.h>

// Node database includes
#include <H3D/Group.h>
#include <H3D/Transform.h>
#include <H3D/Shape.h>
#include <H3D/Coordinate.h>
#include <H3D/Appearance.h>
#include <H3D/Material.h>
#include <H3D/IndexedFaceSet.h>
#include <H3D/FrictionalSurface.h>
#include <H3D/ImageTexture.h>
#include <H3D/TextureTransform.h>
#include <H3D/TextureCoordinate.h>
#include <H3D/FitToBoxTransform.h>


#include <H3D/PointLight.h>
#include <H3D/DirectionalLight.h>

#include <H3D/ComposedShader.h>
#include <H3D/ShaderPart.h>

#include <H3D/PixelTexture.h>

#include <H3D/IndexedTriangleSet.h>
#include <H3D/Normal.h>

#include <H3D/FloatVertexAttribute.h>

#include <H3D/CoordinateInterpolator.h>
#include <H3D/NormalInterpolator.h>

//New
#include <H3D/TimeSensor.h>
#include <H3D/SmoothSurface.h>
#include <H3D/ToggleGroup.h>

//TODO:Rustam: what to do with Images?!
//#include <H3D/MFImage.h>



namespace H3D {

struct NodeFieldWrap {
  bool isField;
  Field *field;
  Node *pParent;
};



class X3DToBinary{
private:
  
  //TiXmlDocument doc;
  std::string filePath;
  H3D::X3D::DEFNodes* DEF_map;
  H3D::X3D::DEFNodes* exported_nodes;

  const std::string VERSION;

  std::map<std::string, unsigned int> Occurrences;

  std::fstream myfile;

  AutoRefVector< H3DScriptNode > script_nodes;

  //TimeStamp startTime, endTime, timeACCattrib, timeACCNodeNameCheck, timeACCDatabase;
  
  std::vector<H3DNodeDatabase *> NodePointers;


  struct USE_NODE {
    bool USE, COPY;
    std::string nodeToUse;
    std::string defName;
  } useNode;
 
  struct Attribute {
    unsigned int attributeNameLength;
    std::string attribName;
    unsigned int numberOfValues;
    char valueType;
    std::string sValue;
    int* iValues;
    double* dValues;
    Attribute(unsigned int attributeNameLength, std::string attributeName, unsigned int numVals, char valType, std::string stringVal, double * doubleVals, int * intVals) : numberOfValues(numVals), valueType(valType), sValue(stringVal), dValues(doubleVals), iValues(intVals) {
    }
    ~Attribute() {
      delete[] iValues;
      delete[] dValues;
    }
  };
  void encode_to_binary_file_ID( TiXmlNode* pParent);
  int encode_attribs_to_binary_file_ID(TiXmlElement* pElement);
  void encode_to_binary_cdata(TiXmlNode* pParent);



  int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
  const char * getIndentAlt( unsigned int numIndents );
  const char * getIndent( unsigned int numIndents );

  //bool BothAreSpaces(char lhs, char rhs);

  void initialiseNodeDB();

  void handleRouteElement(unsigned int numAttribs, bool route_no_event  );
  void handleFieldElement( unsigned int numAttribs, unsigned int numChildren, NodeFieldWrap* parent);
  void handleImportElement(unsigned int numAttribs);
  void handleExportElement(unsigned int numAttribs);
  void handleCdataElement(NodeFieldWrap* parent);
  void handleProgramSettingElement( unsigned int numAttrib );

  Node* parseNodesH3D(NodeFieldWrap * pParent, bool isRoot);
  void parseAttributesH3D(H3D::Node * pNewNode, std::string * pContainerField);
  Attribute* getNextAttribute();

  
  H3D::Node* parseNodesH3DRoot();
  
  void calculateOccurrences (TiXmlNode* pParent);



void parseNodes();
void parseAttributes();


  
public:
  X3DToBinary(X3D::DEFNodes * _def_map, X3D::DEFNodes * _exported_nodes);
  ~X3DToBinary();
  
  void setFilePath(const std::string& fileToLoad);
  int writeToBinary (std::string fileToLoad);
  void readBinary (std::string fileToLoad);
  H3D::Node* readBinaryRoot(std::string fileToLoad);
  void printOccurrences (std::string fileToLoad);
 };

}


#endif