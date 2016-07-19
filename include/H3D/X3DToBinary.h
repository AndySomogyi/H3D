#ifndef __X3DToBinary_H__
#define __X3DToBinary_H__


#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif


#ifdef H3D_WINDOWS
#include <Windows.h>
#endif // H3D_WINDOWS
#include <sys/stat.h>

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
  bool is_field;
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
  const std::string EXT;

  std::map<std::string, unsigned int> Occurrences;

  std::fstream myfile;

  AutoRefVector< H3DScriptNode > script_nodes;

  //TimeStamp startTime, endTime, timeACCattrib, timeACCNodeNameCheck, timeACCDatabase;
  
  std::vector<H3DNodeDatabase *> node_db_pointers;


  struct USE_NODE {
    bool use_attrib, copy_attrib;
    std::string node_to_use;
    std::string def_name;
  } useNode;
 
  struct Attribute {
    unsigned int attribute_name_length;
    std::string attrib_name;
    unsigned int number_of_values;
    char value_type;
    std::string s_value;
    int* i_values;
    double* s_values;
    ~Attribute() {
		switch (value_type)
		{
		case 'i':
			delete[] i_values;
			break;
		case 'd':
			delete[] s_values;
			break;
		default:
			break;
		}
    
  
    }
  };

  void writeNodeToFile( TiXmlNode* p_parent);
  int writeAttributesToFile(TiXmlElement* p_element);
  int writeAttributesToFile(TiXmlElement* p_element, H3D::Node * pParent);
  void writeCDATAtoFile(TiXmlNode* p_parent);
  void initialiseNodeDB();

  Node* parseNodesH3D(NodeFieldWrap * pParent, bool isRoot);
  void parseAttributesH3D(H3D::Node * pNewNode, std::string * p_container_field_name);
  void readNextAttribute(Attribute & a);
  void handleRouteElement(unsigned int num_of_attribs, bool route_no_event  );
  void handleFieldElement( unsigned int num_of_attribs, unsigned int num_of_children, NodeFieldWrap* pParent);
  void handleImportElement(unsigned int num_of_attribs);
  void handleExportElement(unsigned int num_of_attribs);
  void handleCdataElement(NodeFieldWrap* parent);
  void handleProgramSettingElement( unsigned int num_of_attribs );



  void calculateOccurrences (TiXmlNode* pParent);

  
public:
  X3DToBinary(X3D::DEFNodes * _def_map, X3D::DEFNodes * _exported_nodes);
  ~X3DToBinary();
  
  bool openToRead(std::string &file_path);
  bool isX3DModified(std::string &file_path);
  void setFilePath(const std::string& file_to_load);
  int writeToBinary (std::string file_to_load);
  H3D::Node* readBinaryRoot(std::string file_to_load);
  void printOccurrences (std::string file_to_load);
 };

}


#endif