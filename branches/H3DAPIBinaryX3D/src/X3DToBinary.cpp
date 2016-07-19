#include <H3D/X3DToBinary.h>
using namespace H3D;
namespace BUTILS {
bool icompare_pred(unsigned char a, unsigned char b)
{
    return tolower(a) == tolower(b);
}

bool icompare(std::string const& a, std::string const& b)
{
    if (a.length()==b.length()) {
        return std::equal(b.begin(), b.end(),
                           a.begin(), icompare_pred);
    }
    else {
        return false;
    }
}

std::string trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t")
{
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

std::vector<double> getDoublesFromString(const std::string& str, const std::string delim = " " ) {

  std::vector<double> values; 
  values.reserve(str.size()); 
  auto strBegin = str.find_first_not_of(delim);
  while (strBegin != std::string::npos){
    const auto strEnd = str.find_first_of(delim, strBegin+1);
    auto strRange = strEnd - strBegin  + 1;
    if(strEnd == std::string::npos) { 
      strRange = str.size()  - strBegin;
      values.push_back(std::stod(str.substr(strBegin, strRange)));
      break;
    }
   
   values.push_back(std::stod(str.substr(strBegin, strRange)));
    strBegin = str.find_first_not_of(delim, strEnd+1);
  }
  return values;
}

int getNumberOfChildren(TiXmlNode* pParent) {
  TiXmlNode* pChild;
  int numOfChildren = 0;
  for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
  {
    if(pChild->Type() != TiXmlNode::TINYXML_COMMENT && pChild->Type() != TiXmlNode::TINYXML_UNKNOWN)
      numOfChildren++;
  }
  return numOfChildren;
}
int getNumberOfAttributes(TiXmlElement* pElement) {
  int numOfAttrib = 0;
  for (TiXmlAttribute* pAttrib=pElement->FirstAttribute(); pAttrib; pAttrib = pAttrib->Next()) {
    if(pAttrib->ValueStr().size() != 0)
      numOfAttrib ++;
  }
  return numOfAttrib;
}

template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   flip_pair<A,B>);
    return dst;
}
std::string readString(std::fstream &stream, unsigned int  num_bytes)
{
    std::string result(num_bytes, ' ');
    stream.read(&result[0], num_bytes);

    return result;
}

}

namespace H3D {
  void X3DToBinary::calculateOccurrences (TiXmlNode* p_parent) {
   std::map<std::string, unsigned int> ::iterator it;
   TiXmlNode* pChild;
   int t = p_parent->Type();
   switch(t) {
     	case TiXmlNode::TINYXML_UNKNOWN:


	case TiXmlNode::TINYXML_TEXT:

	case TiXmlNode::TINYXML_DECLARATION:
    cout << "T is: " << p_parent->ValueStr();
   }
   if (t == TiXmlNode::TINYXML_ELEMENT) {
   it = Occurrences.find(p_parent->ValueStr());
   if(it == Occurrences.end())
     Occurrences[p_parent->ValueStr()] = 1;
   else 
     it->second ++;
   for ( pChild = p_parent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
   {
     calculateOccurrences( pChild);
   }
   }
}

void X3DToBinary::printOccurrences (std::string file_to_load) {
  TiXmlDocument doc;
  doc.LoadFile(file_to_load);
  TiXmlElement* element = doc.RootElement();
     TiXmlNode* pParent = (TiXmlNode*) element;
     calculateOccurrences(pParent);
     multimap<unsigned int, std::string> map = BUTILS::flip_map(Occurrences);
  for (multimap<unsigned int,std::string>::iterator it=map.begin(); it!=map.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
}

int X3DToBinary::encode_attribs_to_binary_file_ID(TiXmlElement* p_element)
{
  if (!p_element) return 0;

  TiXmlAttribute* p_attrib = p_element->FirstAttribute();
  int i = 0;
  int i_val;
  double d_val;

  while (p_attrib)
  {
	  if (p_attrib->ValueStr().size() == 0) {
		  p_attrib = p_attrib->Next();
		  continue;
	  }

    int length = strlen(p_attrib->Name());

    if (p_attrib->QueryDoubleValue(&d_val) == TIXML_SUCCESS || p_attrib->QueryIntValue(&i_val) == TIXML_SUCCESS) {
      std::string str(p_attrib->Value());

      std::string chars_to_clear(" \t\n\v\f\r,");

      std::vector<double> double_values = BUTILS::getDoublesFromString(str, chars_to_clear);

      int nuv_values = double_values.size();

      //skip elements with no attributes
      if (nuv_values == 0) {
        p_attrib = p_attrib->Next();
        continue;
      }

      myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
      myfile.write(p_attrib->Name(), length);

      if (p_attrib->QueryDoubleValue(&d_val) == TIXML_SUCCESS) {

        int numVal = double_values.size();
        myfile.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
        myfile.write("d", sizeof(char));
        for (unsigned long long int i = 0; i < double_values.size(); i++) {
          double num = double_values[i];
          myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
        }
      }
      else {

        myfile.write(reinterpret_cast<const char *>(&nuv_values), sizeof(int));
        myfile.write("i", sizeof(char));
        for (unsigned long long int i = 0; i < double_values.size(); i++) {
          double num = double_values[i];
          myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
        }
      }


    }
    else { // end if iteger 

      unsigned int number_of_bytes = p_attrib->ValueStr().length();
      if (number_of_bytes == 0) {
        p_attrib = p_attrib->Next();
        continue;
      }
      myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
      myfile.write(p_attrib->Name(), length);
      myfile.write(reinterpret_cast<const char *>(&number_of_bytes), sizeof(int));
      myfile.write("s", sizeof(char));
      myfile.write(p_attrib->Value(), number_of_bytes * sizeof(char));


    }// end IF is double or int 

    i++;
    p_attrib = p_attrib->Next();
  }
  return i;
}
void X3DToBinary::encode_to_binary_cdata(TiXmlNode* p_parent) {
    int length = 5; // sizeof("cdata")
    int ID = 250;
    int num_atr = 1;
    int num_children = 0;
    unsigned int num_of_bytes = 0;
    myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
    myfile.write("cdata", length * sizeof(char));
 
    myfile.write(reinterpret_cast<const char *>(&ID), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&num_atr), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&num_children), sizeof(int));
    num_of_bytes = p_parent->ValueStr().size();
    myfile.write(reinterpret_cast<const char *>(&num_of_bytes), sizeof(int));
    myfile.write("s", sizeof(char));
    myfile.write(p_parent->Value(), num_of_bytes * sizeof(char)); 
}
void X3DToBinary::encode_to_binary_file_ID( TiXmlNode* p_parent)
{
	if ( !p_parent ) return;

	TiXmlNode* p_child;
	int t = p_parent->Type();

  //myfile.write((char*) &tmpIndent, sizeof(tmpIndent));
  int num_of_children,length, num_of_attribs, num_of_bytes;
	int num;
  int ID;
	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		break;

	case TiXmlNode::TINYXML_ELEMENT:
    length = p_parent->ValueStr().length();
    num_of_attribs = BUTILS::getNumberOfAttributes(p_parent->ToElement());
    num_of_children = BUTILS::getNumberOfChildren(p_parent);
    myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
    myfile.write(p_parent->Value(), length * sizeof(char)); 

    ID = -1;
    if (p_parent->ValueStr() == "Scene" || p_parent->ValueStr() == "Group" ){
      ID = 0;
    } else if (p_parent->ValueStr() == "Transform") {
      ID = 1;
    } else if (p_parent->ValueStr() == "Shape") {
      ID = 2;
    } else if (p_parent->ValueStr() == "Coordinate") {
      ID = 3;
    } else if (p_parent->ValueStr() == "Appearance") {
      ID = 4;
    } else if (p_parent->ValueStr() == "Material") {
      ID = 5;
    } else if (p_parent->ValueStr() == "IndexedFaceSet") {
      ID = 6;
    } else if (p_parent->ValueStr() == "FrictionalSurface") {
      ID = 7;
    } else if (p_parent->ValueStr() == "ImageTexture") {
      ID = 8;
    } else if (p_parent->ValueStr() == "TextureTransform") {
      ID = 9;
    } else if (p_parent->ValueStr() == "TextureCoordinate") {
      ID = 10;
    } else if (p_parent->ValueStr() == "FitToBoxTransform") {
      ID = 11;
    }else if (p_parent->ValueStr() == "PointLight") {
      ID = 12;
    } else if (p_parent->ValueStr() == "DirectionalLight") {
      ID = 13;
    } else if (p_parent->ValueStr() == "ComposedShader") {
      ID = 14;
    } else if (p_parent->ValueStr() == "ShaderPart") {
      ID = 15;
    } else if (p_parent->ValueStr() == "PixelTexture") {
      ID = 16;
    } else if (p_parent->ValueStr() == "IndexedTriangleSet") {
      ID = 17;
    } else if (p_parent->ValueStr() == "Normal") {
      ID = 18;
    } else if (p_parent->ValueStr() == "FloatVertexAttribute") {
      ID = 19;
    }else if (p_parent->ValueStr() == "CoordinateInterpolator") {
      ID = 20;
    } else if (p_parent->ValueStr() == "NormalInterpolator") {
      ID = 21;
    }else if (p_parent->ValueStr() == "TimeSensor") {
      ID = 22;
    }else if (p_parent->ValueStr() == "SmoothSurface") {
      ID = 23;
    } else if (p_parent->ValueStr() == "ToggleGroup") {
      ID = 24;
    } else if (p_parent->ValueStr() == "field") {
      ID = 249;
    }else if (p_parent->ValueStr() == "EXPORT") {
      ID = 248;
    }else if (p_parent->ValueStr() == "IMPORT") {
      ID = 247;
    } else if (p_parent->ValueStr() == "ROUTE_WITH_EVENT") {
      ID = 246;
    } else if (p_parent->ValueStr() == "ROUTE_NO_EVENT") {
      ID = 245;
    } else if (p_parent->ValueStr() == "ROUTE") {
      ID = 244;
    } else if (p_parent->ValueStr() == "PROGRAM_SETTING") {
      ID = 243;
    } 
    myfile.write(reinterpret_cast<const char *>(&ID), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&num_of_attribs), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&num_of_children), sizeof(int));
    H3D::Node* pNewNode;

    if (ID < 243 && ID > -2) {
      if (ID == -1) {
        pNewNode = H3D::H3DNodeDatabase::createNode(p_parent->ValueStr());
      }
      else {
        pNewNode = NodePointers[ID]->createNode();
      }
      if (pNewNode) {
        pNewNode->setManualInitialize(true);
        num = encode_attribs_to_binary_file_ID_Node(p_parent->ToElement(), pNewNode);
      } 
      else
        num = encode_attribs_to_binary_file_ID(p_parent->ToElement());
    }
    else {
      num = encode_attribs_to_binary_file_ID(p_parent->ToElement());
    }

	case TiXmlNode::TINYXML_COMMENT:
		//myfile << "Comment: [" << pParent->Value() << "]";
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		//myfile << "Unknown" ;
		break;

	case TiXmlNode::TINYXML_TEXT:
    encode_to_binary_cdata(p_parent);
		//myfile << "Text: [" << pText->Value() << "]";
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		//myfile << "Declaration";
		break;
	default:
		break;
	}
	//myfile << "\n";



	for ( p_child = p_parent->FirstChild(); p_child != 0; p_child = p_child->NextSibling()) 
	{
		encode_to_binary_file_ID(p_child);
	}
  
  
  //myfile << "\n" <<  ">" << pParent->ValueStr().length() << pParent->Value();
  //myfile << "\n" <<  ">";
}
int X3DToBinary::encode_attribs_to_binary_file_ID_Node(TiXmlElement* p_element, H3D::Node * p_parent)
{
  if (!p_element) return 0;
  TiXmlAttribute* p_attrib = p_element->FirstAttribute();
  int i = 0;
  int i_val;
  double d_val;
  int field_type;
  H3D::Field * pField;

  while (p_attrib)
  {
	  if (p_attrib->ValueStr().size() == 0) {
		  p_attrib = p_attrib->Next();
		  continue;
	  }
		  


    int length = strlen(p_attrib->Name());
    pField = p_parent->getField(p_attrib->Name());
    if (pField) {
      field_type = pField->getX3DType();
      if (field_type != H3D::X3DTypes::UNKNOWN_X3D_TYPE)
      {
        if (field_type == H3D::X3DTypes::SFBOOL
          || field_type == H3D::X3DTypes::SFSTRING
          || field_type == H3D::X3DTypes::MFBOOL
          || field_type == H3D::X3DTypes::MFBOOL
          || field_type == H3D::X3DTypes::MFSTRING)
        {
          unsigned int numberOfBytes = p_attrib->ValueStr().length();
          if (numberOfBytes == 0) {
            p_attrib = p_attrib->Next();
            continue;
          }
          myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
          myfile.write(p_attrib->Name(), length);
          myfile.write(reinterpret_cast<const char *>(&numberOfBytes), sizeof(int));
          myfile.write("s", sizeof(char));
          myfile.write(p_attrib->Value(), numberOfBytes * sizeof(char));

        }
        // DOUBLES
        else
        {
          std::string str(p_attrib->Value());
          std::string chars_to_Clear(" \t\n\v\f\r,");
          std::vector<double> double_values = BUTILS::getDoublesFromString(str, chars_to_Clear);
          int numVal = double_values.size();
          myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
          myfile.write(p_attrib->Name(), length);
          myfile.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
          myfile.write("d", sizeof(char));
          for (unsigned long long int i = 0; i < double_values.size(); i++) {
            double num = double_values[i];
            myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
          }
        }
      }
    }
    else {
      if (p_attrib->QueryDoubleValue(&d_val) == TIXML_SUCCESS || p_attrib->QueryIntValue(&i_val) == TIXML_SUCCESS) {
        std::string str(p_attrib->Value());

        std::string charToFill(" ");
        std::string charsToClear(" \t\n\v\f\r,");

        std::vector<double> valuesD = BUTILS::getDoublesFromString(str, charsToClear);

        int numVal = valuesD.size();

        //skip elements with no attributes
        if (numVal == 0) {
          p_attrib = p_attrib->Next();
          continue;
        }

        myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
        myfile.write(p_attrib->Name(), length);

        if (p_attrib->QueryDoubleValue(&d_val) == TIXML_SUCCESS) {

          int numVal = valuesD.size();
          myfile.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
          myfile.write("d", sizeof(char));
          for (unsigned long long int i = 0; i < valuesD.size(); i++) {
            double num = valuesD[i];
            myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
          }
        }
        else {

          myfile.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
          myfile.write("i", sizeof(char));
          for (unsigned long long int i = 0; i < valuesD.size(); i++) {
            double num = valuesD[i];
            myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
          }
        }


      }
      else { // end if iteger 

        unsigned int number_of_bytes = p_attrib->ValueStr().length();
        if (number_of_bytes == 0) {
          p_attrib = p_attrib->Next();
          continue;
        }
        myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
        myfile.write(p_attrib->Name(), length);
        myfile.write(reinterpret_cast<const char *>(&number_of_bytes), sizeof(int));
        myfile.write("s", sizeof(char));
        myfile.write(p_attrib->Value(), number_of_bytes * sizeof(char));


      }// end IF is double or int 
    }
    i++;
    p_attrib = p_attrib->Next();
  }
  return i;
}

int X3DToBinary::writeToBinary (std::string file_to_load = "") {
  if(file_to_load.empty()) file_to_load = filePath;
  TiXmlDocument doc;
  doc.LoadFile(file_to_load);
  std::string pathToWrite = file_to_load + ".inprogress";
  std::remove(pathToWrite.c_str());
  std::remove((file_to_load + EXT).c_str());
  if (myfile.is_open())
    myfile.close();
  myfile.open (pathToWrite, std::fstream::out | std::fstream::app | std::ios::binary);
   if ( doc.ErrorId() == 0 ) {
     //put a flag at a very beginning to indicate if the file has been written successfuly

     std::string header = "" + VERSION;
     int header_length = header.size();
     myfile.write(reinterpret_cast<const char*> (&header_length), sizeof(int));
     myfile.write(header.c_str(), header_length *sizeof(char));

     TiXmlElement* element = doc.RootElement();
     TiXmlNode* p_parent = (TiXmlNode*) element;
     std::string name = p_parent->Value();
     // TODO:Rustam: Think over this code. How to find the root element for the binary file? Top element may be both <Scene> and not <scene> 
     bool root_found = false;
     if (name != "Group" && name != "Scene" ) {
       for ( TiXmlNode* pChild=element->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
       {	
         std::string name = pChild->Value();
         if (name == "Group" || name == "Scene" ) {
           encode_to_binary_file_ID( pChild );
           root_found = true;
           break;
         }
       }
       if(!root_found) {
         encode_to_binary_file_ID( p_parent );

       }
     } else {
       encode_to_binary_file_ID( p_parent );
     }
   }
   else {
	   myfile.close();
	   Console(LogLevel::Warning) << "H3DB: TinyXML could not load: " << file_to_load << std::endl;
	   return 1;
   }

  /* myfile.seekp(myfile.beg);
   myfile.write("<\"\"", 3 * sizeof(char));*/
   myfile.close();
   if (rename(pathToWrite.c_str(), (file_to_load + EXT).c_str()) == 0)
     Console(LogLevel::Info) << "File successfully written and renamed. " << file_to_load + EXT << std::endl;
   else 
     Console(LogLevel::Error) << "File could not be renamed: " << pathToWrite << std::endl;
   return 0;
}

template <typename T>
void setFieldValue (H3D::ParsableField *pPField, unsigned int field_type, T *t, unsigned int num_of_elements) {

  switch(field_type) {
  case H3D::X3DTypes::SFFLOAT:
    if (H3D::SFFloat* f = static_cast<H3D::SFFloat*>(pPField)){
      f->setValue(t[0]);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFDOUBLE:
     if (H3D::SFDouble* f = static_cast<H3D::SFDouble*>(pPField)){
      f->setValue(t[0]);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFTIME:
    if (H3D::SFTime* f = static_cast<H3D::SFTime*>(pPField)){
      f->setValue(t[0]);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFINT32:
    if (H3D::SFInt32* f = static_cast<H3D::SFInt32*>(pPField)){
      f->setValue(t[0]);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFVEC2F:
     if (H3D::SFVec2f* f = static_cast<H3D::SFVec2f*>(pPField)){
       f->setValue(H3D::ArithmeticTypes::Vec2f(t[0], t[1]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFVEC2D:
    if (H3D::SFVec2d* f = static_cast<H3D::SFVec2d*>(pPField)){
       f->setValue(H3D::ArithmeticTypes::Vec2d(t[0], t[1]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFVEC3F:
    if(H3D::SFVec3f* f = static_cast<H3D::SFVec3f*>(pPField)) {
      f->setValue(H3D::ArithmeticTypes::Vec3f(t[0], t[1], t[2]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFVEC3D:
    if(H3D::SFVec3d* f = static_cast<H3D::SFVec3d*>(pPField)) {
      f->setValue(H3D::ArithmeticTypes::Vec3d(t[0], t[1], t[2]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFVEC4F:
    if(H3D::SFVec4f* f = static_cast<H3D::SFVec4f*>(pPField)) {
      f->setValue(H3D::ArithmeticTypes::Vec4f(t[0], t[1], t[2], t[3]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFVEC4D:
    if(H3D::SFVec4d* f = static_cast<H3D::SFVec4d*>(pPField)) {
      f->setValue(H3D::ArithmeticTypes::Vec4d(t[0], t[1], t[2], t[3]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
    
  case H3D::X3DTypes::SFCOLOR:
    if (H3D::SFColor* f = static_cast<H3D::SFColor*>(pPField)){
      f->setValue(RGB(t[0], t[1],t[2]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFCOLORRGBA:
     if (H3D::SFColorRGBA* f = static_cast<H3D::SFColorRGBA*>(pPField)){
      f->setValue(RGBA(t[0], t[1],t[2],t[3]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFROTATION:
    if (H3D::SFRotation* f = static_cast<H3D::SFRotation*>(pPField)){
      f->setValue(H3D::ArithmeticTypes::Rotation(H3D::ArithmeticTypes::Vec3f(t[0], t[1], t[2]), t[3]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFQUATERNION:
    if (H3D::SFQuaternion* f = static_cast<H3D::SFQuaternion*>(pPField)){
      f->setValue(H3D::ArithmeticTypes::Quaternion(t[0], t[1], t[2], t[3]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFMATRIX3F:
    //TODO:Rustam: how are matrices given in X3D?
     if (H3D::SFMatrix3f* f = static_cast<H3D::SFMatrix3f*>(pPField)){
      f->setValue(H3D::ArithmeticTypes::Matrix3f(t[0], t[1], t[2],
                                                   t[3], t[4], t[5],
                                                   t[6], t[7], t[8]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFMATRIX4F:
     //TODO:Rustam: how are matrices given in X3D?
     if (H3D::SFMatrix4f* f = static_cast<H3D::SFMatrix4f*>(pPField)){
      f->setValue(H3D::ArithmeticTypes::Matrix4f(t[0], t[1], t[2], t[3],
                                                   t[4], t[5], t[6], t[7],
                                                   t[8], t[9], t[10], t[11],
                                                   t[12], t[13], t[14], t[15]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFMATRIX3D:
     //TODO:Rustam: how are matrices given in X3D?
     if (H3D::SFMatrix3d* f = static_cast<H3D::SFMatrix3d*>(pPField)){
      f->setValue(H3D::ArithmeticTypes::Matrix3d(t[0], t[1], t[2],
                                                   t[3], t[4], t[5],
                                                   t[6], t[7], t[8]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFMATRIX4D:
    //TODO:Rustam: how are matrices given in X3D?
     if (H3D::SFMatrix4d* f = static_cast<H3D::SFMatrix4d*>(pPField)){
      f->setValue(H3D::ArithmeticTypes::Matrix4d(t[0], t[1], t[2], t[3],
                                                   t[4], t[5], t[6], t[7],
                                                   t[8], t[9], t[10], t[11],
                                                   t[12], t[13], t[14], t[15]));
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::SFIMAGE:
    //TODO:Rustam: what to do with images?!
    break;

// =================== MF types =============================

  case H3D::X3DTypes::MFFLOAT:
    if (H3D::MFFloat* f = static_cast<H3D::MFFloat*>(pPField)){
      std::vector<H3D::H3DFloat> vals;
      for (int i=0; i < num_of_elements; i++){
        vals.push_back(t[i]);
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFDOUBLE:
    if (H3D::MFDouble* f = static_cast<H3D::MFDouble*>(pPField)){
      std::vector<H3D::H3DDouble> vals;
      for (int i=0; i < num_of_elements; i++){
        vals.push_back(t[i]);
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFTIME:
    if (H3D::MFTime* f = static_cast<H3D::MFTime*>(pPField)){
      std::vector<H3D::H3DTime> vals;
      for (int i=0; i < num_of_elements; i++){
        vals.push_back(t[i]);
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFINT32:
    if (H3D::MFInt32* f = static_cast<H3D::MFInt32*>(pPField)){
      std::vector<H3D::H3DInt32> vals;
      for (int i=0; i < num_of_elements; i++){
        vals.push_back(t[i]);
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFVEC2F:
    if (H3D::MFVec2f* f = static_cast<H3D::MFVec2f*>(pPField)){
         std::vector<H3D::Vec2f> vals;
      for (int i=0; i < num_of_elements; i+=2){
        vals.push_back(H3D::Vec2f(t[i],t[i+1]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFVEC2D:
    if (H3D::MFVec2d* f = static_cast<H3D::MFVec2d*>(pPField)){
         std::vector<H3D::Vec2d> vals;
      for (int i=0; i < num_of_elements; i+=2){
        vals.push_back(H3D::Vec2d(t[i],t[i+1]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFVEC3F:
    if (H3D::MFVec3f* f = static_cast<H3D::MFVec3f*>(pPField)){
      std::vector<H3D::Vec3f> vals;
      for(int i = 0; i < num_of_elements; i+=3){
        vals.push_back(H3D::Vec3f(t[i],t[i+1],t[i+2]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFVEC3D:
      if (H3D::MFVec3d* f = static_cast<H3D::MFVec3d*>(pPField)){
      std::vector<H3D::Vec3d> vals;
      for(int i = 0; i < num_of_elements; i+=3){
        vals.push_back(H3D::Vec3d(t[i],t[i+1],t[i+2]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFVEC4F:
     if (H3D::MFVec4f* f = static_cast<H3D::MFVec4f*>(pPField)){
      std::vector<H3D::Vec4f> vals;
      for(int i = 0; i < num_of_elements; i+=4){
        vals.push_back(H3D::Vec4f(t[i],t[i+1],t[i+2],t[i+3]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFVEC4D:
     if (H3D::MFVec4d* f = static_cast<H3D::MFVec4d*>(pPField)){
      std::vector<H3D::Vec4d> vals;
      for(int i = 0; i < num_of_elements; i+=4){
        vals.push_back(H3D::Vec4d(t[i],t[i+1],t[i+2],t[i+3]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFCOLOR:
    if (H3D::MFColor* f = static_cast<H3D::MFColor*>(pPField)){
      std::vector<H3D::RGB> vals;
      for(int i = 0; i < num_of_elements; i+=3){
        vals.push_back(H3D::RGB(t[i],t[i+1],t[i+2]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFCOLORRGBA:
     if (H3D::MFColorRGBA* f = static_cast<H3D::MFColorRGBA*>(pPField)){
       std::vector<H3D::RGBA> vals;
       for(int i = 0; i < num_of_elements; i+=4){
         vals.push_back(H3D::RGBA(t[i],t[i+1],t[i+2],t[i+3]));
       }
       f->swap(vals);
     } else {
       std::cout << "Could not cast" << std::endl;
     }
    break;
  case H3D::X3DTypes::MFROTATION:
    if (H3D::MFRotation* f = static_cast<H3D::MFRotation*>(pPField)){
      std::vector<H3D::Rotation> vals;
       for(int i = 0; i < num_of_elements; i+=4){
         vals.push_back(H3D::Rotation(t[i],t[i+1],t[i+2],t[i+3]));
       }
       f->swap(vals);
     } else {
       std::cout << "Could not cast" << std::endl;
     }
    break;
  case H3D::X3DTypes::MFQUATERNION:
       if (H3D::MFQuaternion* f = static_cast<H3D::MFQuaternion*>(pPField)){
         std::vector<H3D::Quaternion> vals;
       for(int i = 0; i < num_of_elements; i+=4){
         vals.push_back(H3D::Quaternion(t[i],t[i+1],t[i+2],t[i+3]));
       }
       f->swap(vals);
     } else {
       std::cout << "Could not cast" << std::endl;
     }
    break;
  case H3D::X3DTypes::MFMATRIX3F:
    if (H3D::MFMatrix3f* f = static_cast<H3D::MFMatrix3f*>(pPField)){
      std::vector<H3D::Matrix3f> vals;
      for(int i = 0; i < num_of_elements; i+=9){
        vals.push_back(H3D::ArithmeticTypes::Matrix3f(t[0], t[1], t[2],
                                                      t[3], t[4], t[5],
                                                      t[6], t[7], t[8]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFMATRIX4F:
    if (H3D::MFMatrix4f* f = static_cast<H3D::MFMatrix4f*>(pPField)){
      std::vector<H3D::Matrix4f> vals;
      for(int i = 0; i < num_of_elements; i+=16){
        vals.push_back(H3D::ArithmeticTypes::Matrix4f(t[0], t[1], t[2], t[3],
                                                      t[4], t[5], t[6], t[7],
                                                      t[8], t[9], t[10], t[11],
                                                      t[12], t[13], t[14], t[15]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFMATRIX3D:
     if (H3D::MFMatrix3d* f = static_cast<H3D::MFMatrix3d*>(pPField)){
      std::vector<H3D::Matrix3d> vals;
      for(unsigned int i = 0; i < num_of_elements; i+=9){
        vals.push_back(H3D::ArithmeticTypes::Matrix3d(t[0], t[1], t[2],
                                                      t[3], t[4], t[5],
                                                      t[6], t[7], t[8]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFMATRIX4D:
     if (H3D::MFMatrix4d* f = static_cast<H3D::MFMatrix4d*>(pPField)){
      std::vector<H3D::Matrix4d> vals;
      for(unsigned int i = 0; i < num_of_elements; i+=16){
        vals.push_back(H3D::ArithmeticTypes::Matrix4d(t[0], t[1], t[2], t[3],
                                                      t[4], t[5], t[6], t[7],
                                                      t[8], t[9], t[10], t[11],
                                                      t[12], t[13], t[14], t[15]));
      }
      f->swap(vals);
    } else {
      std::cout << "Could not cast" << std::endl;
    }
    break;
  case H3D::X3DTypes::MFIMAGE:
    //TODO:Rustam: what tod o with IMAGES ? :(
    break;
  case H3D::X3DTypes::UNKNOWN_X3D_TYPE:
  default:
    std::cout<<"Ooooops. Field type: " << field_type << " does not exist" << std::endl; 
    break;

  } //END of switch(enumType)
}

void setFieldValue(H3D::ParsableField *pPField, unsigned int field_type, std::string s_value){
  bool test = false;
  switch(field_type) {
    case H3D::X3DTypes::SFBOOL:
      if (H3D::SFBool* f = static_cast<H3D::SFBool*>(pPField))
        f->setValue(BUTILS::icompare(s_value, "true"));
    break;

  case H3D::X3DTypes::SFSTRING:
    if(H3D::SFString* f = static_cast<H3D::SFString*>(pPField))
      f->setValue(s_value);
    break;
  case H3D::X3DTypes::MFBOOL:
    pPField->setValueFromString(s_value);
    break;
  case H3D::X3DTypes::MFSTRING:
    pPField->setValueFromString(s_value);
  break;
  case H3D::X3DTypes::UNKNOWN_X3D_TYPE :
  default:
    break;

  }
}

void X3DToBinary::parseAttributesH3D(H3D::Node* pNewNode, std::string* p_container_field_name) {
	Attribute attrib;
	readNextAttribute(attrib);
  if(attrib.attrib_name == "USE" ) {
    useNode.use_attrib = true;
    useNode.node_to_use= attrib.s_value;
    } else if (attrib.attrib_name == "COPY" ) {
     //TODO:Rusta: should it be casesensitive?
     if(attrib.s_value == "true")
       useNode.copy_attrib = true;

  // else it should be left FALSE
  } else if (attrib.attrib_name == "DEF") {
  if( pNewNode && DEF_map ) {
        useNode.def_name = attrib.s_value;
        DEF_map->addNode(attrib.s_value, pNewNode );
        pNewNode->setName(attrib.s_value);
        
  } 

  } else if(attrib.attrib_name == "class" ) {
     // class is a reserved attribute name 
  
   }
   else if (attrib.attrib_name == "containerField") {
     *p_container_field_name = attrib.s_value;
   } else {
     H3D::Field * pField = pNewNode->getField(attrib.attrib_name);
     
     //TODO:Rustam check this
    /* std::string typeName = pField->getTypeName();*/
     
     unsigned int fieldType;
     /// test->setValueFromString
     if ( !pField ) {
       std::cout << "Warning: Couldn't find field named \"" << attrib.attrib_name
         << "\" in " << pNewNode->getName() << " node " << std::endl;
     }
     else {
       H3D::ParsableField *pPField = dynamic_cast< H3D::ParsableField * >( pField );
       if( !pPField ) {
         std::cout << "Cannot parse field \"" << attrib.attrib_name
           << "\". Field type must be a subclass of ParsableField "
           << "in order to be parsable. ";
       } else {

       
         fieldType = pPField->getX3DType();
         switch (attrib.value_type) {
         case 'i':
           setFieldValue(pPField, fieldType, attrib.i_values, attrib.number_of_values);
           break;
         case 'd':
           setFieldValue(pPField, fieldType, attrib.s_values, attrib.number_of_values);
           break;
         case 's':
           /* s_value = readString(myfile, number_of_values);*/
           setFieldValue(pPField, fieldType, attrib.s_value);
           break;
           
         default:
           break;
         }

 
         //cout << tSetValue << std::endl;
       }
     }
   } // end of parsing Field
   

 /*
     TODO:Rustam 
      Allocations are Expensive
    Sure, there were quite a few improvements with allocation during last 10 years 
    (including so-called thread-caching in tcmalloc and per-thread arenas in ptmalloc2), 
    but well – it is still a rather slow operation (give or take, we’re speaking about 
    the order of magnitude of 200 CPU clocks per allocation/deallocation pair, 
    and more if you’re not so lucky with your allocator).
 */

}

void X3DToBinary::handleRouteElement(unsigned int num_of_attribs, bool route_no_event  ) {
  Attribute attrib;
  std::string from_node_name = "", to_node_name = "", from_field_name = "", to_field_name = "";
  for( int i = 0; i < num_of_attribs; ++i ) {
	  readNextAttribute(attrib);
  if( from_node_name.empty() && attrib.attrib_name == "fromNode" ) {
      from_node_name = attrib.s_value;
    } else if( to_node_name.empty() && attrib.attrib_name == "toNode" ) {
		to_node_name = attrib.s_value;
    } else if( from_field_name.empty() && attrib.attrib_name == "fromField" ) {
      from_field_name = attrib.s_value;
    } else if( to_field_name.empty() && attrib.attrib_name == "toField" ) {
		to_field_name = attrib.s_value;
    } else {
		Console(LogLevel::Warning) << "H3DB: Warning: Unknown attribute \"" << attrib.attrib_name
           << "\" in ROUTE element " << endl;
    }
  }

   // All route attributes were not found, so whrow exception
  if( from_node_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"fromNode\" attribute ";
	Console(LogLevel::Error) << "H3DB: "<< message << attrib.attrib_name
           << "\" in ROUTE element " << endl;
  } else if ( from_field_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"fromField\" attribute ";
    Console(LogLevel::Error) << "H3DB: " << message << attrib.attrib_name
           << "\" in ROUTE element " << endl;
  } else if ( to_node_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"toNode\" attribute ";
    Console(LogLevel::Error) << "H3DB: " << message << attrib.attrib_name
           << "\" in ROUTE element " << endl;
  }
  else if ( to_field_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"toField\" attribute ";
    Console(LogLevel::Error) << message << attrib.attrib_name
           << "\" in ROUTE element " << endl;
  } else {
    // Lookup the nodes and fields and set up the route.
    Node *from_node = DEF_map->getNode( from_node_name);
    if( from_node ) {
      Field *from_field = 
        from_node->getField( from_field_name );
      if( from_field ) {
        Node *to_node = DEF_map->getNode( to_node_name );
        if( to_node ) {
          Field *to_field = 
            to_node->getField( to_field_name );
          if( to_field ) {
            if( !route_no_event )
              from_field->route( to_field );
            else
              from_field->routeNoEvent( to_field );
          } else {
            Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find field named \""
				<< to_field_name 
				<< "\" in \"" 
				<< to_node_name
				<< "\" Node " 
				<< filePath 
				<< endl;
          }
        } else {
          Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find Node named \""
               << to_node_name
               << "\" specified in \"toNode\" attribute " 
			   << filePath
               << endl;
        }
      } else {
        Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find field named \""
             << from_field_name
             << "\" in \"" << from_node_name << "\" Node " 
			<< filePath
			<< endl;
      }
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find Node named \""
		  << from_node_name
		  << "\" specified in \"fromNode\" attribute " 
		  << filePath
		  << endl;
    }
  }
}

void X3DToBinary::handleFieldElement(unsigned int num_of_attribs, unsigned int num_of_children, NodeFieldWrap* parent ) {
  char field_value_type;

  Attribute attrib;

  std::string field_name, field_type, field_access_type;
  bool has_field_value = false;

    for( unsigned int i = 0; i < num_of_attribs; ++i ) {
		readNextAttribute(attrib);
		if( field_name.empty() && attrib.attrib_name == "name" ) {
			field_name = attrib.s_value;
		} else if( field_type.empty() && attrib.attrib_name == "type" ) {
			field_type = attrib.s_value;
		} else if( field_access_type.empty() && attrib.attrib_name == "accessType" ) {
			field_access_type = attrib.s_value;
		} else if(attrib.attrib_name == "value" ) {
			has_field_value = true;
			field_value_type = attrib.value_type;
		} else {
        Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
			<< "\" in 'field' element " << endl;
		}
	}
	H3DDynamicFieldsObject *dyn_object = 
      dynamic_cast< H3DDynamicFieldsObject * >( parent->pParent );
    if( dyn_object ) {
    // All field attributes were not found, so throw exception
     /* if( field_name.empty() ) {
      string message = 
        "Invalid 'field' specification. Missing \"name\" attribute";
       Console(LogLevel::Warning) << message << std::endl;
      } else if ( field_type.empty() ) {
      string message = 
        "Invalid 'field' specification. Missing \"type\" attribute";
       Console(LogLevel::Warning)<< message << std::endl;
      } else if ( field_access_type.empty() ) {
      string message = 
        "Invalid 'field' specification. Missing \"accessType\" attribute";
      Console(LogLevel::Warning)<< message << std::endl;
    } */
      Field *f = H3D::X3DTypes::newFieldInstance(field_type.c_str() );
    
    if( !f ) {
      string message = 
        "Invalid value for 'type' attribute of 'field' specification.";
      Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
    }
    
    f->setOwner( parent->pParent );
    f->setName( field_name );

    string access_type_string = field_access_type;
    Field::AccessType access_type;
    if( access_type_string == "initializeOnly" ) 
      access_type= Field::INITIALIZE_ONLY;
    else if( access_type_string == "outputOnly" )
      access_type = Field::OUTPUT_ONLY;
    else if( access_type_string == "inputOnly" )
      access_type = Field::INPUT_ONLY;
    else if( access_type_string == "inputOutput" )
      access_type = Field::INPUT_OUTPUT;
    else {
      string message = 
        "Invalid value for 'accessType' attribute of 'field' specification.";
      Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
    }
    
    if( has_field_value ) {
      ParsableField *pfield = 
        dynamic_cast< ParsableField * >( f );
      if( !pfield ) {
        stringstream s;
        s << "Cannot parse field \"" << f->getFullName() 
          << "\". Field type must be a subclass of ParsableField "
          << "in order to be parsable. ";
        Console(LogLevel::Warning) << "H3DB: " << s.str() << std::endl;
      }
      if( access_type == Field::INITIALIZE_ONLY ||
          access_type == Field::INPUT_OUTPUT ||
          access_type == Field::INPUT_ONLY ) {
 
            unsigned int field_type_id = pfield->getX3DType();
            switch(field_value_type){
            case 'i':
              setFieldValue (pfield, field_type_id, attrib.i_values, attrib.number_of_values);
              break;
            case 'd':
              setFieldValue (pfield, field_type_id, attrib.s_values, attrib.number_of_values);
              break;
            case 's':
              setFieldValue(pfield, field_type_id, attrib.s_value);
              break;
            }

      } else {
        Console(LogLevel::Warning) << "H3DB: " << "Warning: 'value' attribute ignored. Only used if "
             << "accesstype is initializeOnly, inputOnly or inputOutput " 
             << endl;
      }
    }
    dyn_object->addField(  field_name ,
                          access_type,
                          f );
    //FieldValue *fv = new FieldValue( field_name, f );
    //node_stack.push( NodeElement( fv ) ); 
    //return f;
    NodeFieldWrap* nfw = new NodeFieldWrap();
    nfw->field = f;
    nfw->is_field = true;
    nfw->pParent = parent->pParent;
    for (unsigned int i = 0; i < num_of_children; i++) {
      Node * pDump = parseNodesH3D(nfw, false);
    }
  }  else {
    Console(LogLevel::Warning) << "H3DB: " << "Warning: 'field' declaration in Node that is does not support"
         << " it " << endl;
    //node_stack.push( NodeElement( NULL ) ); 
  }

}
void X3DToBinary::handleImportElement(unsigned int num_of_attribs){
  
  
  char fieldValueType;
  std::string inline_def_name, exported_def_name, as_name;
  Attribute attrib;
  for( unsigned int i = 0; i < num_of_attribs; ++i ) {
	  readNextAttribute(attrib);
    if( inline_def_name.empty() && attrib.attrib_name == "inlineDEF" ) {
      inline_def_name = attrib.s_value;
    } else if( exported_def_name.empty() && attrib.attrib_name == "exportedDEF" ) {
      exported_def_name = attrib.s_value;
    } else if( as_name.empty() && attrib.attrib_name == "AS" ) {
      as_name = attrib.s_value;
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
             << "\" in 'field' element " << endl;
    }
  }
  
  if( inline_def_name.empty() ) {
    string message = 
        "Invalid 'field' specification. Missing \"name\" attribute";
    Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
  } else if ( exported_def_name.empty() ) {
    string message = 
        "Invalid 'field' specification. Missing \"type\" attribute";
    Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
  } else if ( as_name.empty() ) {
    string message = 
        "Invalid 'field' specification. Missing \"accessType\" attribute";
    Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
  } else {
    
    const std::string &s = exported_def_name;
    if( inline_def_name == "H3D_EXPORTS" ) {
      Node *import_node = 
        H3D::H3DExports::getH3DExportNode( s );
      if( import_node ) {
        DEF_map->addNode(as_name, 
          import_node ); 
      } else {
        Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. H3D_EXPORTS "
          << "does not include \""
          << exported_def_name << "\"" 
          << endl;
      }
    } else {
      Node *n = DEF_map->getNode( inline_def_name );
      Inline *inline_node = dynamic_cast< Inline * >( n );
      if( inline_node ) {
        Node *import_node = 
          inline_node->exported_nodes.getNode( exported_def_name );
        if( import_node ) {
          DEF_map->addNode(as_name, 
                            import_node); 
        } else {
          Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. Inline node \""
               << inline_def_name << "\" does not EXPORT \""
               << exported_def_name << "\"" 
               << endl;
        }
      } else {
        if( n ) {
          Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. Node \""
               << inline_def_name << "\" is not an Inline node "
               << endl;
        } else {
          Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. Node named \""
               << inline_def_name << "\" does not exist."
               << endl;
        }
      }
    }

  }
}
void X3DToBinary::handleExportElement( unsigned int numAttribs ) {
  char fieldValueType;

  std::string local_def_name, as_name;
  Attribute attrib;
  for( unsigned int i = 0; i < numAttribs; ++i ) {
	  readNextAttribute(attrib);
	  if( local_def_name.empty() && attrib.attrib_name == "localDEF" ) {
      local_def_name = attrib.s_value;
    } else if( as_name.empty() && attrib.attrib_name == "AS" ) {
      as_name = attrib.s_value;
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
             << " in EXPORT element" << endl;
    }
  }
  if( local_def_name.empty() ) {
    string message = 
        "Invalid 'EXPORT' specification. Missing \"localDEF\" attribute";
    Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
  }  else if ( as_name.empty() ) {
    string message = 
        "Invalid 'IMPORT' specification. Missing \"AS\" attribute";
    Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
  } else {
    // Lookup the nodes and fields and set up the route.
    Node *n = DEF_map->getNode( local_def_name);
    if( n ) {
      exported_nodes->addNode(as_name, n ); 
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Warning: EXPORT error. Node named \""
           << local_def_name << "\" does not exist."
           << endl;
    }
  }
  // push NULL on the node stack to skip elements within EXPORT element.
  //node_stack.push( NodeElement( NULL ) );
}

//Reads special element CDATA. Binary Write handles writing Cdata in a special way. Be carefull changes in writing have to be reflected in reading.
void X3DToBinary::handleCdataElement(NodeFieldWrap* pParent) {
  int num_of_values = -1;
  char value_type = ' '; //Can be either 'i', 'd', 's'
  std::string s_value;

  myfile.read((char *) &num_of_values, sizeof(int));
  myfile.read(&value_type, sizeof(char));
  s_value = BUTILS::readString(myfile, num_of_values);
  if( X3DUrlObject *url_object = 
    dynamic_cast< X3DUrlObject * >( pParent->pParent ) ) {
      url_object->url->push_back( s_value );
  }
}

void X3DToBinary::handleProgramSettingElement( unsigned int num_of_attribs ) {
  Attribute attrib;
  std::string node_name, field_name, setting_name, setting_section;
  setting_name = ""; setting_section = "Main settings";
  
  for( unsigned int i = 0; i < num_of_attribs; ++i ) {
	  readNextAttribute(attrib);

    if( node_name.empty() && attrib.attrib_name == "node" ) {
      node_name = attrib.s_value;
    } else if( field_name.empty() && attrib.attrib_name == "field" ) {
      field_name = attrib.s_value;
    } else if(attrib.attrib_name == "name" ) {
      setting_name = attrib.s_value;
    } else if(attrib.attrib_name == "section" ) {
      setting_section = attrib.s_value;
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
             << " in PROGRAM_SETTING element" << endl;
    }
  }
        
            
  // All route attributes were not found, so whrow exception
  if( node_name.empty() ) {
    Console(LogLevel::Warning) << "H3DB: " << "Invalid PROGRAM_SETTING specification. Missing \"node\" attribute" << std::endl;
    
  } else if ( field_name.empty() ) {
   Console(LogLevel::Warning) << "H3DB: " << "Invalid PROGRAM_SETTING specification. Missing \"field\" attribute" << std::endl;
  } else {
    // Lookup the nodes and fields and set up the route.
    Node *node = DEF_map->getNode(node_name);
    if( node ) {
      Field *field = 
        node->getField( field_name.c_str() );
      if( field ) {
        Scene::addProgramSetting( field, 
                                  setting_name,
                                  setting_section  ); 
                                  
      } else {
        Console(LogLevel::Warning) << "H3DB: " << "Warning: Program setting error. Could not find field named \""
             << field_name
             << "\" in \"" << node_name << "\" Node " 
             << endl;
      }
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Warning: program setting error. Could not find Node named \""
           << node_name
           << "\" specified in \"node\" attribute " 
            << endl;
    }
  }

  // push NULL on the node stack to skip elements within PROGRAM_SETTING element.
}

void X3DToBinary::readNextAttribute(Attribute &a)
{
	myfile.read((char *)&a.attribute_name_length, sizeof(int));
	a.attrib_name = BUTILS::readString(myfile, a.attribute_name_length);
	myfile.read((char *)&a.number_of_values, sizeof(int));
	myfile.read(&a.value_type, sizeof(char));

	switch (a.value_type) {
	case 'i':
		a.i_values = new int[a.number_of_values];
		myfile.read((char*)a.i_values, a.number_of_values * sizeof(int));

		break;

	case 'd':
		a.s_values = new double[a.number_of_values];
		myfile.read((char*)a.s_values, a.number_of_values * sizeof(double));

		break;
	case 's':
		a.s_value = BUTILS::readString(myfile, a.number_of_values);
		break;
	default:
		break;
	}
}
void X3DToBinary::initialiseNodeDB() {
  //Order is important corresponds to our encoding.
  NodePointers.push_back(&Group::database);
  NodePointers.push_back(&Transform::database);
  NodePointers.push_back(&Shape::database);
  NodePointers.push_back(&Coordinate::database);
  NodePointers.push_back(&Appearance::database);
  NodePointers.push_back(&Material::database);
  NodePointers.push_back(&IndexedFaceSet::database);
  NodePointers.push_back(&FrictionalSurface::database);
  NodePointers.push_back(&ImageTexture::database);
  NodePointers.push_back(&TextureTransform::database);
  NodePointers.push_back(&TextureCoordinate::database);
  NodePointers.push_back(&FitToBoxTransform::database);
  //NodePointers.push_back(Shape::database);
  //NodePointers.push_back(Material::database);

  NodePointers.push_back(&PointLight::database);
  NodePointers.push_back(&DirectionalLight::database);
  NodePointers.push_back(&ComposedShader::database);
  NodePointers.push_back(&ShaderPart::database);
  NodePointers.push_back(&PixelTexture::database);
  NodePointers.push_back(&IndexedTriangleSet::database);

  NodePointers.push_back(&Normal::database);

  NodePointers.push_back(&FloatVertexAttribute::database);
  NodePointers.push_back(&CoordinateInterpolator::database);
  NodePointers.push_back(&NormalInterpolator::database);

  
  NodePointers.push_back(&TimeSensor::database);
  NodePointers.push_back(&SmoothSurface::database);
  NodePointers.push_back(&ToggleGroup::database);
}

Node* X3DToBinary::parseNodesH3D(NodeFieldWrap* pParent, bool isRoot) {
	// start = now()
	int node_name_length = -1;
	int num_of_attribs = -1;
	int num_of_children = -1;
	int node_id = -1;
	std::string node_name;
	H3D::Node* pNewNode;
	myfile.read((char*)&node_name_length, sizeof(int));
	node_name = BUTILS::readString(myfile, node_name_length);
	//myfile.read(nodeName, nodeNameSize);
	//nodeName[nodeNameSize] = '\0';
	myfile.read((char*)&node_id, sizeof(int));
	myfile.read((char*)&num_of_attribs, sizeof(int));
	myfile.read((char*)&num_of_children, sizeof(int));

	//TimeStamp startTNameCheck = TimeStamp::now();
	bool event;
	GlobalSettings *p_glob_settings;
	switch (node_id) {
	case 243: //Program_Setting
		handleProgramSettingElement(num_of_attribs);
		break;
	case 244: //ROUTE
		event = GlobalSettings::default_x3d_route_sends_event;
		p_glob_settings = GlobalSettings::getActive();
		if (p_glob_settings) event = p_glob_settings->x3dROUTESendsEvent->getValue();
		handleRouteElement(num_of_attribs, !event);
		break;
	case 245: //Route_No_Event
		handleRouteElement(num_of_attribs, true);
		break;
	case 246: //ROUTE_WITH_EVENT
		handleRouteElement(num_of_attribs, false);
		break;
	case 247: //IMPORT
		handleImportElement(num_of_attribs);
		break;
	case 248: //EXPORT
		handleExportElement(num_of_attribs);
		break;
	case 249: //field
		handleFieldElement(num_of_attribs, num_of_children, pParent);
		break;
	case 250: //CDATA
		handleCdataElement(pParent);
		break;
	default:
		//TimeStamp endTNameCheck = TimeStamp::now();
		// timeACCNodeNameCheck = timeACCNodeNameCheck + endTNameCheck - startTNameCheck;
		try {
			//TimeStamp startTDatabase = TimeStamp::now();
			if (node_id == -1) {
				pNewNode = H3D::H3DNodeDatabase::createNode(node_name);
			}
			else {
				pNewNode = NodePointers[node_id]->createNode();
			}
			//TimeStamp endTDatabase = TimeStamp::now();
			//timeACCDatabase = timeACCDatabase + endTDatabase - startTDatabase;

			if (pNewNode) {
				// remove the initialization that occurs on first reference
				// since we don't want it to occur until all child nodes
				// have been created and set.
				pNewNode->setManualInitialize(true);

				// if node is a script node add the current named nodes
				// from the current DEF_map and store the node in the 
				// script_nodes vector. When parsing is done the named
				// nodes will be updated in all scripts to contain
				// all named nodes from the parsed file.

				if (H3DScriptNode *script_node =
					dynamic_cast< H3DScriptNode * >(pNewNode)) {
					script_node->addNamedNodes(DEF_map);
					script_nodes.push_back(script_node);
				}
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: Could not create \"" << node_name
					<< "\" node. It does not exist in the H3DNode Database " << std::endl;
			}
		}
		catch (const H3D::Exception::H3DException &e) {
			Console(LogLevel::Warning) << "H3DB: " << "Warning: Could not create \"" << node_name
				<< "\" node. Exception in constructor: "
				<< e << std::endl;
		}

		std::string containerFieldName = pNewNode ? pNewNode->defaultXMLContainerField() : std::string("");
		useNode.use_attrib = false;
		useNode.copy_attrib = false;

		for (int i = 0; i < num_of_attribs; ++i) {
			// TimeStamp startTattr = TimeStamp::now();
			parseAttributesH3D(pNewNode, &containerFieldName);
			//TimeStamp endTAttr = TimeStamp::now();
			//timeACCattrib = timeACCattrib + (endTAttr-startTattr);
		}

		if (useNode.use_attrib) {
			// if we have a USE attribute, lookup the matching node and use that.
			pNewNode = DEF_map->getNode(useNode.node_to_use);
			if (containerFieldName.empty())
				containerFieldName = pNewNode->defaultXMLContainerField();
			if (useNode.copy_attrib && pNewNode)
			{
				if (!useNode.def_name.empty())
				{
					Node *copied_node = pNewNode->clone();
					DEF_map->addNode(useNode.def_name, copied_node);
					copied_node->setName(useNode.def_name);
					pNewNode = copied_node;
				}
				else
				{
					Console(LogLevel::Warning) << "H3DB: " << "Invalid COPY attribute. "
						<< "No DEF node name defined.";
				}
			}

			if (!pNewNode) {
				Console(LogLevel::Warning) << "H3DB: " << "Invalid USE attribute. "
					<< "No node with DEF name \"" << useNode.def_name
					<< "\" defined.";
			}
		}

		NodeFieldWrap* p_nwf = new NodeFieldWrap();
		p_nwf->pParent = pNewNode;
		p_nwf->is_field = false;
		
		for (int i = 0; i < num_of_children; ++i)
			Node * pDump = parseNodesH3D(p_nwf, false);
		
		if (!pNewNode->isInitialized() && pNewNode->getManualInitialize())
			pNewNode->initialize();

		//startTime = TimeStamp::now();
		if (pParent->is_field) {
			Field::AccessType access_type = pParent->field->getAccessType();
			if (access_type == Field::INITIALIZE_ONLY ||
				access_type == Field::INPUT_OUTPUT) {
				SFNode *sf = dynamic_cast< SFNode *>(pParent->field);
				if (sf) {
					sf->setValue(pNewNode);
				}
				else {
					MFNode *mf = dynamic_cast< MFNode *>(pParent->field);
					if (mf) {
						mf->push_back(pNewNode);
					}
					else Console(LogLevel::Warning) << "H3DB: " << "Warning: 'value' element ignored. Only used if "
						<< "field type is SFNode or MFNode"
						<< endl;
				}
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: 'value' element ignored. Only used if "
					<< "accesstype is initializeOnly or inputOutput "
					<< endl;
			}
		}
		else {
			H3D::Field *pField = pParent->pParent->getField(containerFieldName);
			if (pField) {
				H3D::SFNode *pSFNode = dynamic_cast< H3D::SFNode *>(pField);
				if (pSFNode) {
					pSFNode->setValue(pNewNode);
				}
				else {
					H3D::MFNode *pMFNode = dynamic_cast< H3D::MFNode *>(pField);
					if (pMFNode) {
						pMFNode->push_back(pNewNode);
					}
				}
			}
			else { // (!f) Could not get field
				if (!isRoot) {
					Console(LogLevel::Error) << "H3DB: " << "Invalid containerField attribute \""
						<< containerFieldName << ". Parent: " << pParent->pParent->getName() << std::endl << " New Node: " << pNewNode->getFullName();
				}
			}

		}
		//endTime = TimeStamp::now();
		//TimeStamp tSetField = endTime - startTime;
	}

	//TODO:Rustam: this has to be tested!
	if (node_name == "Inline") {
		Inline *inline_node = dynamic_cast< Inline * >(pNewNode);

		if (inline_node) {
			const string &import_mode = inline_node->importMode->getValue();
			if (import_mode == "AUTO" || import_mode == "AUTO_IMPORT") {
				DEF_map->merge(&inline_node->exported_nodes);
			}
		}
	}

	if (!isRoot)
	{
		return nullptr;
	}
	else
	{
		return pNewNode;
	}
	//std::cout << nodeNameSize << nodeName;
	//end = now()
	// map[nodeName] = end - start;
}
H3D::Node* X3DToBinary::readBinaryRoot(std::string file_to_load){
  if (!myfile.is_open())
        myfile.open(file_to_load + EXT, std::ios::binary | std::ifstream::in);

  int header_length;
  if(myfile.is_open()){
    std::string file_version;
    myfile.read((char *) &header_length, sizeof(int));
    file_version = BUTILS::readString(myfile, header_length);
    if(file_version == VERSION) {
      NodeFieldWrap* pNFW = new NodeFieldWrap();
      pNFW->pParent = new Node();
      pNFW->field = false;
      Node* rootNode = parseNodesH3D(pNFW, true);
      
      myfile.close();
      return rootNode;
    } else {
      Console(LogLevel::Warning) << "H3DB: " << "Could not load. File Version: * " << file_version << " * is old. Current Version: " << VERSION << std::endl;
      return NULL;
    }
  }
  Console(LogLevel::Error) << "H3DB: " << "Could not load BINARY file: " << file_to_load << EXT << std::endl;
  return NULL;
  
}
void X3DToBinary::setFilePath(const std::string& file_to_load){
  filePath = file_to_load;
}

bool X3DToBinary::openToRead(std::string &file_path) {
	std::string base = ResourceResolver::getBaseURL();
	//if (base != "objects\tools" && base != "x3d\eyes\components" && base != "x3d/scenes/../eyes/components/")
	//{
	//	Console(LogLevel::Warning) << "H3DB: " << "Omitting files in: " << base << std::endl;
	//	return false;
	//}
	//Console(LogLevel::Warning) << "H3DB: " << "Opened files in: " << base << std::endl;
	std::string binary_file_path = file_path + EXT;
	if (!isX3DModified(file_path)) {
		myfile.open(binary_file_path, std::ios::binary | std::ifstream::in);
		return myfile.is_open();
	}
	return false;
}
bool X3DToBinary::isX3DModified(std::string &file_path) {
  struct _stat orig_file_stat, cache_file_stat;
  if (_stat(file_path.c_str(), &orig_file_stat) != 0) 
    return true;
  if (_stat((file_path + EXT).c_str(), &cache_file_stat) != 0) 
    return true;
  if (cache_file_stat.st_mtime > orig_file_stat.st_mtime)
    return false;
  return true;
}
X3DToBinary::X3DToBinary(X3D::DEFNodes * _def_map, X3D::DEFNodes * _exported_nodes) : VERSION("H3DB01"), EXT(".h3db"), DEF_map(_def_map), exported_nodes(_exported_nodes) {
  if(!DEF_map) {
    DEF_map = new X3D::DEFNodes();
  }
  if( !_exported_nodes ) {
    exported_nodes = new X3D::DEFNodes();
  }
  initialiseNodeDB();
}
}


