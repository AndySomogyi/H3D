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
std::string readString(std::fstream &stream, unsigned int  numBytes)
{
    std::string result(numBytes, ' ');
    stream.read(&result[0], numBytes);

    return result;
}

}

namespace H3D {
  void X3DToBinary::calculateOccurrences (TiXmlNode* pParent) {
   std::map<std::string, unsigned int> ::iterator it;
   TiXmlNode* pChild;
   int t = pParent->Type();
   switch(t) {
     	case TiXmlNode::TINYXML_UNKNOWN:


	case TiXmlNode::TINYXML_TEXT:

	case TiXmlNode::TINYXML_DECLARATION:
    cout << "T is: " << pParent->ValueStr();
   }
   if (t == TiXmlNode::TINYXML_ELEMENT) {
   it = Occurrences.find(pParent->ValueStr());
   if(it == Occurrences.end())
     Occurrences[pParent->ValueStr()] = 1;
   else 
     it->second ++;
   for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
   {
     calculateOccurrences( pChild);
   }
   }
}

void X3DToBinary::printOccurrences (std::string fileToLoad) {
  TiXmlDocument doc;
  doc.LoadFile(fileToLoad);
  TiXmlElement* element = doc.RootElement();
     TiXmlNode* pParent = (TiXmlNode*) element;
     calculateOccurrences(pParent);
     multimap<unsigned int, std::string> map = BUTILS::flip_map(Occurrences);
  for (multimap<unsigned int,std::string>::iterator it=map.begin(); it!=map.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
}

int X3DToBinary::encode_attribs_to_binary_file_ID(TiXmlElement* pElement)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;

	while (pAttrib)
	{
    int length = strlen(pAttrib->Name());

    if(pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS || pAttrib->QueryIntValue(&ival) == TIXML_SUCCESS) {
      std::string str(pAttrib->Value());
      
      std::string charToFill(" ");
      std::string charsToClear(" \t\n\v\f\r,");
      
      std::vector<double> valuesD = BUTILS::getDoublesFromString(str, charsToClear);
      
      int numVal = valuesD.size();
      
      //skip elements with no attributes
      if (numVal == 0) {
        pAttrib=pAttrib->Next();
         continue;
      }
       
      myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
      myfile.write(pAttrib->Name(), length);

      if(pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS) {

        int numVal = valuesD.size();
        myfile.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
        myfile.write("d", sizeof(char));
        for(unsigned long long int i = 0; i < valuesD.size(); i++) {
           double num = valuesD[i];
          myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
        }
      }
      else {

        myfile.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
        myfile.write("i", sizeof(char));
        for(unsigned long long int i = 0; i < valuesD.size(); i++) {
           double num = valuesD[i];
          myfile.write(reinterpret_cast<const char *>(&num), sizeof(double));
        }
      }
      
    
    } else { // end if iteger 

      unsigned int numberOfBytes = pAttrib->ValueStr().length();
      if (numberOfBytes == 0) {
        pAttrib=pAttrib->Next();
        continue;
      }
      myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
      myfile.write(pAttrib->Name(), length);
      myfile.write(reinterpret_cast<const char *>(&numberOfBytes), sizeof(int));
      myfile.write("s", sizeof(char));
      myfile.write(pAttrib->Value(), numberOfBytes * sizeof(char));


    }// end IF is double or int 
    
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;	
}
void X3DToBinary::encode_to_binary_cdata(TiXmlNode* pParent) {
    int length = 5; // sizeof("cdata")
    int ID = 250;
    int numAtr = 1;
    int numChildren = 0;
    unsigned int numOfBytes = 0;
    myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
    myfile.write("cdata", length * sizeof(char));
 
    myfile.write(reinterpret_cast<const char *>(&ID), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&numAtr), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&numChildren), sizeof(int));
    numOfBytes = pParent->ValueStr().size();
    myfile.write(reinterpret_cast<const char *>(&numOfBytes), sizeof(int));
    myfile.write("s", sizeof(char));
    myfile.write(pParent->Value(), numOfBytes * sizeof(char)); 
}
void X3DToBinary::encode_to_binary_file_ID( TiXmlNode* pParent)
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();

  //myfile.write((char*) &tmpIndent, sizeof(tmpIndent));
  int numChildren,length, numAtr, numOfBytes;
	int num;
  int ID;
	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		break;

	case TiXmlNode::TINYXML_ELEMENT:
    length = pParent->ValueStr().length();
    numAtr = BUTILS::getNumberOfAttributes(pParent->ToElement());
    numChildren = BUTILS::getNumberOfChildren(pParent);
    myfile.write(reinterpret_cast<const char *>(&length), sizeof(int));
    myfile.write(pParent->Value(), length * sizeof(char)); 

    ID = -1;
    if (pParent->ValueStr() == "Scene" || pParent->ValueStr() == "Group" ){
      ID = 0;
    } else if (pParent->ValueStr() == "Transform") {
      ID = 1;
    } else if (pParent->ValueStr() == "Shape") {
      ID = 2;
    } else if (pParent->ValueStr() == "Coordinate") {
      ID = 3;
    } else if (pParent->ValueStr() == "Appearance") {
      ID = 4;
    } else if (pParent->ValueStr() == "Material") {
      ID = 5;
    } else if (pParent->ValueStr() == "IndexedFaceSet") {
      ID = 6;
    } else if (pParent->ValueStr() == "FrictionalSurface") {
      ID = 7;
    } else if (pParent->ValueStr() == "ImageTexture") {
      ID = 8;
    } else if (pParent->ValueStr() == "TextureTransform") {
      ID = 9;
    } else if (pParent->ValueStr() == "TextureCoordinate") {
      ID = 10;
    } else if (pParent->ValueStr() == "FitToBoxTransform") {
      ID = 11;
    }else if (pParent->ValueStr() == "PointLight") {
      ID = 12;
    } else if (pParent->ValueStr() == "DirectionalLight") {
      ID = 13;
    } else if (pParent->ValueStr() == "ComposedShader") {
      ID = 14;
    } else if (pParent->ValueStr() == "ShaderPart") {
      ID = 15;
    } else if (pParent->ValueStr() == "PixelTexture") {
      ID = 16;
    } else if (pParent->ValueStr() == "IndexedTriangleSet") {
      ID = 17;
    } else if (pParent->ValueStr() == "Normal") {
      ID = 18;
    } else if (pParent->ValueStr() == "FloatVertexAttribute") {
      ID = 19;
    }else if (pParent->ValueStr() == "CoordinateInterpolator") {
      ID = 20;
    } else if (pParent->ValueStr() == "NormalInterpolator") {
      ID = 21;
    }else if (pParent->ValueStr() == "TimeSensor") {
      ID = 22;
    }else if (pParent->ValueStr() == "SmoothSurface") {
      ID = 23;
    } else if (pParent->ValueStr() == "ToggleGroup") {
      ID = 24;
    } else if (pParent->ValueStr() == "field") {
      ID = 249;
    }else if (pParent->ValueStr() == "EXPORT") {
      ID = 248;
    }else if (pParent->ValueStr() == "IMPORT") {
      ID = 247;
    } else if (pParent->ValueStr() == "ROUTE_WITH_EVENT") {
      ID = 246;
    } else if (pParent->ValueStr() == "ROUTE_NO_EVENT") {
      ID = 245;
    } else if (pParent->ValueStr() == "ROUTE") {
      ID = 244;
    } else if (pParent->ValueStr() == "PROGRAM_SETTING") {
      ID = 243;
    } 

    myfile.write(reinterpret_cast<const char *>(&ID), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&numAtr), sizeof(int));
    myfile.write(reinterpret_cast<const char *>(&numChildren), sizeof(int));
		num=encode_attribs_to_binary_file_ID(pParent->ToElement());

	case TiXmlNode::TINYXML_COMMENT:
		//myfile << "Comment: [" << pParent->Value() << "]";
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		//myfile << "Unknown" ;
		break;

	case TiXmlNode::TINYXML_TEXT:
    encode_to_binary_cdata(pParent);
		//myfile << "Text: [" << pText->Value() << "]";
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		//myfile << "Declaration";
		break;
	default:
		break;
	}
	//myfile << "\n";



	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		encode_to_binary_file_ID( pChild);
	}
  
  
  //myfile << "\n" <<  ">" << pParent->ValueStr().length() << pParent->Value();
  //myfile << "\n" <<  ">";
}


int X3DToBinary::writeToBinary (std::string fileToLoad = "") {
  if(fileToLoad.empty()) fileToLoad = filePath;
  TiXmlDocument doc;
  doc.LoadFile(fileToLoad);
  string::size_type to = fileToLoad.find_last_of( "/\\" );

  std::string basePath;
  if( to != string::npos ) {
    basePath = fileToLoad.substr( 0, to + 1 );
  }
  std::string fileName = fileToLoad.substr(to+1, -1);
  std::string pathToWrite = basePath + fileName + ".r";
  remove(pathToWrite.c_str());
  myfile.open (pathToWrite, std::fstream::out | std::fstream::app | std::ios::binary);
   if ( doc.ErrorId() == 0 ) {
     //put a flag at a very beginning to indicate if the file has been written successfuly
     std::string header = "" + VERSION;
     int headerLength = header.size();
     myfile.write(reinterpret_cast<const char*> (&headerLength), sizeof(int));
     myfile.write(header.c_str(), headerLength *sizeof(char));

     TiXmlElement* element = doc.RootElement();
     TiXmlNode* pParent = (TiXmlNode*) element;
     std::string name = pParent->Value();
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
         encode_to_binary_file_ID( pParent );
       }
     } else {
       encode_to_binary_file_ID( pParent );
     }
   }
   else {
     myfile << "Doc is not read correctly \n";
   }
   myfile.close();
   return 0;
}

template <typename T>
void setFieldValue (H3D::ParsableField *pPField, unsigned int fieldType, T *t, unsigned int numberOfElements) {

  switch(fieldType) {
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
      for (int i=0; i < numberOfElements; i++){
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
      for (int i=0; i < numberOfElements; i++){
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
      for (int i=0; i < numberOfElements; i++){
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
      for (int i=0; i < numberOfElements; i++){
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
      for (int i=0; i < numberOfElements; i+=2){
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
      for (int i=0; i < numberOfElements; i+=2){
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
      for(int i = 0; i < numberOfElements; i+=3){
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
      for(int i = 0; i < numberOfElements; i+=3){
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
      for(int i = 0; i < numberOfElements; i+=4){
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
      for(int i = 0; i < numberOfElements; i+=4){
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
      for(int i = 0; i < numberOfElements; i+=3){
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
       for(int i = 0; i < numberOfElements; i+=4){
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
       for(int i = 0; i < numberOfElements; i+=4){
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
       for(int i = 0; i < numberOfElements; i+=4){
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
      for(int i = 0; i < numberOfElements; i+=9){
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
      for(int i = 0; i < numberOfElements; i+=16){
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
      for(unsigned int i = 0; i < numberOfElements; i+=9){
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
      for(unsigned int i = 0; i < numberOfElements; i+=16){
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
    std::cout<<"Ooooops. Field type: " << fieldType << " does not exist" << std::endl; 
    break;

  } //END of switch(enumType)
}

void setFieldValue(H3D::ParsableField *pPField, unsigned int fieldType, std::string sValue){
  bool test = false;
  switch(fieldType) {
    case H3D::X3DTypes::SFBOOL:
      if (H3D::SFBool* f = static_cast<H3D::SFBool*>(pPField))
        f->setValue(BUTILS::icompare(sValue, "true"));
    break;

  case H3D::X3DTypes::SFSTRING:
    if(H3D::SFString* f = static_cast<H3D::SFString*>(pPField))
      f->setValue(sValue);
    break;
  case H3D::X3DTypes::MFBOOL:
    pPField->setValueFromString(sValue);
    break;
  case H3D::X3DTypes::MFSTRING:
    pPField->setValueFromString(sValue);
  break;
  case H3D::X3DTypes::UNKNOWN_X3D_TYPE :
  default:
    break;

  }
}

void X3DToBinary::parseAttributesH3D(H3D::Node* pNewNode, std::string* pContainerField) {
 
  int attribNameSize = -1;
  std::string attribName;
  int numberOfValues = -1;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  int* iValues;
  double* dValues;
  std::string sValue;
  myfile.read((char *) &attribNameSize, sizeof(int));
  attribName = BUTILS::readString(myfile, attribNameSize);
  myfile.read((char *) &numberOfValues, sizeof(int));
  myfile.read(&valueType, sizeof(char));
  std::string s = " ";
  std::stringstream tmp;
  
  switch (valueType) {
  case 'i':
    iValues = new int [numberOfValues];
    myfile.read((char*) iValues,  numberOfValues * sizeof(int));
    break;
  case 'd':
    dValues = new double [numberOfValues];
    myfile.read((char*) dValues, numberOfValues * sizeof(double));
    break;  
  case 's':
    sValue = BUTILS::readString(myfile, numberOfValues);
    break;
  default:
    break;
  }

  if( attribName == "USE" ) {
    useNode.USE = true;
    useNode.nodeToUse=sValue;
    } else if ( attribName == "COPY" ) {
     //TODO:Rusta: should it be casesensitive?
     if(sValue == "true") 
       useNode.COPY = true;

  // else it should be left FALSE
  } else if (attribName == "DEF") {
  if( pNewNode && DEF_map ) {
        useNode.defName = sValue;
        DEF_map->addNode(sValue, pNewNode ); 
        pNewNode->setName(sValue);
        
  } 

  } else if( attribName == "class" ) {
     // class is a reserved attribute name 
  
   }
   else if (attribName == "containerField") {
     *pContainerField = sValue;
   } else {
     H3D::Field * pField = pNewNode->getField(attribName);
     
     //TODO:Rustam check this
    /* std::string typeName = pField->getTypeName();*/
     
     unsigned int fieldType;
     /// test->setValueFromString
     if ( !pField ) {
       std::cout << "Warning: Couldn't find field named \"" << attribName 
         << "\" in " << pNewNode->getName() << " node " << std::endl;
     }
     else {
       H3D::ParsableField *pPField = dynamic_cast< H3D::ParsableField * >( pField );
       if( !pPField ) {
         std::cout << "Cannot parse field \"" << attribName
           << "\". Field type must be a subclass of ParsableField "
           << "in order to be parsable. ";
       } else {

       
         fieldType = pPField->getX3DType();
         switch (valueType) {
         case 'i':
           setFieldValue(pPField, fieldType, iValues, numberOfValues);
           delete [] iValues;
           break;
         case 'd':
           setFieldValue(pPField, fieldType, dValues, numberOfValues);
           delete[] dValues;
           break;
         case 's':
           /* sValue = readString(myfile, numberOfValues);*/
           setFieldValue(pPField, fieldType, sValue);
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

X3DToBinary::Attribute * X3DToBinary::getNextAttribute() {
  unsigned int attributeNameLength = 0;
  std::string attribName;
  unsigned int numberOfValues = 0;
  char valueType = ' ';
  //Can be either 'i', 'd', 's'
  std::string sValue;
  int* iValues;
  double* dValues;
  char fieldValueType;
  X3DToBinary::Attribute * a;
  myfile.read((char *) &attributeNameLength, sizeof(int));
  attribName = BUTILS::readString(myfile, attributeNameLength);
  myfile.read((char *) &numberOfValues, sizeof(int));
  myfile.read(&valueType, sizeof(char));

    switch(valueType) {
    case 'i':
      iValues = new int [numberOfValues];
      myfile.read((char*) iValues,  numberOfValues * sizeof(int));
      a = new X3DToBinary::Attribute(attributeNameLength, attribName, numberOfValues,valueType, nullptr, nullptr, iValues ); 
      break;
      
    case 'd':
      dValues = new double [numberOfValues];
      myfile.read((char*) dValues,  numberOfValues * sizeof(double));
      a = new X3DToBinary::Attribute(attributeNameLength, attribName, numberOfValues,valueType, "", dValues, nullptr ); 
      break;
    case 's':
      sValue = BUTILS::readString(myfile, numberOfValues);
      a = new X3DToBinary::Attribute(attributeNameLength, attribName, numberOfValues,valueType, sValue, nullptr, nullptr ); 
      break;
    default:
      break;
    }
    
    return a;
}

void X3DToBinary::handleRouteElement(unsigned int numAttribs, bool route_no_event  ) {
  
  std::string from_node_name="", to_node_name="", from_field_name="", to_field_name="" ;
  int attribNameSize = -1;
  std::string attribName;
  int numberOfValues = -1;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  std::string sValue;


  for( int i = 0; i < numAttribs; ++i ) {
  
  myfile.read((char *) &attribNameSize, sizeof(int));
  attribName = BUTILS::readString(myfile, attribNameSize);
  myfile.read((char *) &numberOfValues, sizeof(int));
  myfile.read(&valueType, sizeof(char));

  if( from_node_name.empty() && attribName == "fromNode" ) {
      from_node_name = BUTILS::readString(myfile, numberOfValues);
    } else if( to_node_name.empty() && attribName == "toNode" ) {
      to_node_name = sValue = BUTILS::readString(myfile, numberOfValues);
    } else if( from_field_name.empty() && attribName == "fromField" ) {
      from_field_name = sValue = BUTILS::readString(myfile, numberOfValues);
    } else if( to_field_name.empty() && attribName == "toField" ) {
      to_field_name = BUTILS::readString(myfile, numberOfValues);
    } else {
      Console(LogLevel::Warning) << "Warning: Unknown attribute \"" << attribName  
           << "\" in ROUTE element " << endl;
    }
  }

   // All route attributes were not found, so whrow exception
  if( from_node_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"fromNode\" attribute ";
    Console(LogLevel::Error) << message << attribName  
           << "\" in ROUTE element " << endl;
  } else if ( from_field_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"fromField\" attribute ";
    Console(LogLevel::Error) << message << attribName  
           << "\" in ROUTE element " << endl;
  } else if ( to_node_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"toNode\" attribute ";
    Console(LogLevel::Error) << message << attribName  
           << "\" in ROUTE element " << endl;
  }
  else if ( to_field_name.empty() ) {
    string message = "Invalid ROUTE specification. Missing \"toField\" attribute ";
    Console(LogLevel::Error) << message << attribName  
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
            Console(LogLevel::Warning) << "Warning: Route error. Could not find field named \"" 
                 << to_field_name
                 << "\" in \"" << to_node_name << "\" Node " 
                        << endl;
          }
        } else {
          Console(LogLevel::Warning) << "Warning: Route error. Could not find Node named \"" 
               << to_node_name
               << "\" specified in \"toNode\" attribute " 
               << endl;
        }
      } else {
        Console(LogLevel::Warning) << "Warning: Route error. Could not find field named \"" 
             << from_field_name
             << "\" in \"" << from_node_name << "\" Node " 
              << endl;
      }
    } else {
      Console(LogLevel::Warning) << "Warning: Route error. Could not find Node named \"" 
           << from_node_name
           << "\" specified in \"fromNode\" attribute " 
           << endl;
    }
  }
}

void X3DToBinary::handleFieldElement(unsigned int numAttribs, unsigned int numChildren, NodeFieldWrap* parent ) {

  int attribNameSize = -1;
  std::string attribName;
  unsigned int numberOfValues = 0;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  std::string sValue;
  int* iValues;
  double* dValues;
  char fieldValueType;

  std::string field_name, field_type, field_access_type;
  bool fieldValue = false;

    for( unsigned int i = 0; i < numAttribs; ++i ) {

    myfile.read((char *) &attribNameSize, sizeof(int));
    attribName = BUTILS::readString(myfile, attribNameSize);
    myfile.read((char *) &numberOfValues, sizeof(int));
    myfile.read(&valueType, sizeof(char));

    switch(valueType) {
    case 'i':
      iValues = new int [numberOfValues];
      myfile.read((char*) iValues,  numberOfValues * sizeof(int));
     
      break;
      
    case 'd':
      dValues = new double [numberOfValues];
      myfile.read((char*) dValues,  numberOfValues * sizeof(double));
     
      break;
    case 's':
      sValue = BUTILS::readString(myfile, numberOfValues);
      break;
    default:
      break;
    }

    if( field_name.empty() && attribName == "name" ) {
        field_name = sValue;
   } else if( field_type.empty() && attribName == "type" ) {
        field_type = sValue;
   } else if( field_access_type.empty() &&  attribName == "accessType" ) {
        field_access_type = sValue;
   } else if( attribName == "value" ) {
     fieldValue = true;
     fieldValueType = valueType;
      } else {
        Console(LogLevel::Warning) << "Warning: Unknown attribute \"" << attribName  
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
      Console(LogLevel::Warning)<< message << std::endl;
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
      Console(LogLevel::Warning)<< message << std::endl;
    }
    
    if( fieldValue ) {
      ParsableField *pfield = 
        dynamic_cast< ParsableField * >( f );
      if( !pfield ) {
        stringstream s;
        s << "Cannot parse field \"" << f->getFullName() 
          << "\". Field type must be a subclass of ParsableField "
          << "in order to be parsable. ";
        Console(LogLevel::Warning)<< s.str() << std::endl;
      }
      if( access_type == Field::INITIALIZE_ONLY ||
          access_type == Field::INPUT_OUTPUT ||
          access_type == Field::INPUT_ONLY ) {
 
            unsigned int fieldType = pfield->getX3DType();
            switch(fieldValueType){
            case 'i':
              setFieldValue (pfield, fieldType, iValues, numberOfValues); 
              delete[] iValues;
              break;
            case 'd':
              setFieldValue (pfield, fieldType, dValues, numberOfValues); 
              delete[] dValues;
              break;
            case 's':
              setFieldValue(pfield, fieldType,sValue);
              break;
            }

      } else {
        Console(LogLevel::Warning) << "Warning: 'value' attribute ignored. Only used if "
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
    nfw->isField = true;
    nfw->pParent = parent->pParent;
    for (unsigned int i = 0; i < numChildren; i++) {
      Node * pDump = parseNodesH3D(nfw, false);
    }
  }  else {
    Console(LogLevel::Warning) << "Warning: 'field' declaration in Node that is does not support"
         << " it " << endl;
    //node_stack.push( NodeElement( NULL ) ); 
  }

}
void X3DToBinary::handleImportElement(unsigned int numAttribs){
  
  int attribNameSize = -1;
  std::string attribName;
  int numberOfValues = -1;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  std::string sValue;
  int* iValues;
  double* dValues;
  char fieldValueType;

  std::string inline_def_name, exported_def_name, as_name;
  
  for( unsigned int i = 0; i < numAttribs; ++i ) {
    
    myfile.read((char *) &attribNameSize, sizeof(int));
    attribName = BUTILS::readString(myfile, attribNameSize);
    myfile.read((char *) &numberOfValues, sizeof(int));
    myfile.read(&valueType, sizeof(char));

    switch(valueType) {
    case 'i':
      iValues = new int [numberOfValues];
      myfile.read((char*) iValues,  numberOfValues * sizeof(int));
     
      break;
      
    case 'd':
      dValues = new double [numberOfValues];
      myfile.read((char*) dValues,  numberOfValues * sizeof(double));
     
      break;
    case 's':
      sValue = BUTILS::readString(myfile, numberOfValues);
      break;
    default:
      break;
    }

    if( inline_def_name.empty() && attribName == "inlineDEF" ) {
      inline_def_name = sValue;
    } else if( exported_def_name.empty() && attribName == "exportedDEF" ) {
      exported_def_name = sValue;
    } else if( as_name.empty() &&  attribName == "AS" ) {
      as_name = sValue;
    } else {
      Console(LogLevel::Warning) << "Warning: Unknown attribute \"" << attribName  
             << "\" in 'field' element " << endl;
    }
  }
  
  if( inline_def_name.empty() ) {
    string message = 
        "Invalid 'field' specification. Missing \"name\" attribute";
    Console(LogLevel::Warning) << message << std::endl;
  } else if ( exported_def_name.empty() ) {
    string message = 
        "Invalid 'field' specification. Missing \"type\" attribute";
    Console(LogLevel::Warning)<< message << std::endl;
  } else if ( as_name.empty() ) {
    string message = 
        "Invalid 'field' specification. Missing \"accessType\" attribute";
    Console(LogLevel::Warning)<< message << std::endl;
  } else {
    
    const std::string &s = exported_def_name;
    if( inline_def_name == "H3D_EXPORTS" ) {
      Node *import_node = 
        H3D::H3DExports::getH3DExportNode( s );
      if( import_node ) {
        DEF_map->addNode(as_name, 
          import_node ); 
      } else {
        Console(LogLevel::Warning) << "Warning: IMPORT error. H3D_EXPORTS " 
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
          Console(LogLevel::Warning) << "Warning: IMPORT error. Inline node \"" 
               << inline_def_name << "\" does not EXPORT \""
               << exported_def_name << "\"" 
               << endl;
        }
      } else {
        if( n ) {
          Console(LogLevel::Warning) << "Warning: IMPORT error. Node \"" 
               << inline_def_name << "\" is not an Inline node "
               << endl;
        } else {
          Console(LogLevel::Warning) << "Warning: IMPORT error. Node named \"" 
               << inline_def_name << "\" does not exist."
               << endl;
        }
      }
    }

  }
}
void X3DToBinary::handleExportElement( unsigned int numAttribs ) {
  int attribNameSize = -1;
  std::string attribName;
  int numberOfValues = -1;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  std::string sValue;
  int* iValues;
  double* dValues;
  char fieldValueType;

  std::string local_def_name, as_name;
  
  for( unsigned int i = 0; i < numAttribs; ++i ) {
    
    myfile.read((char *) &attribNameSize, sizeof(int));
    attribName = BUTILS::readString(myfile, attribNameSize);
    myfile.read((char *) &numberOfValues, sizeof(int));
    myfile.read(&valueType, sizeof(char));

    switch(valueType) {
    case 'i':
      iValues = new int [numberOfValues];
      myfile.read((char*) iValues,  numberOfValues * sizeof(int));
     
      break;
      
    case 'd':
      dValues = new double [numberOfValues];
      myfile.read((char*) dValues,  numberOfValues * sizeof(double));
     
      break;
    case 's':
      sValue = BUTILS::readString(myfile, numberOfValues);
      break;
    default:
      break;
    }

    if( local_def_name.empty() && attribName == "localDEF" ) {
      local_def_name = sValue;
    } else if( as_name.empty() &&  attribName == "AS" ) {
      as_name = sValue;
    } else {
      Console(LogLevel::Warning) << "Warning: Unknown attribute \"" << attribName  
             << " in EXPORT element" << endl;
    }
  }
            
   
  if( local_def_name.empty() ) {
    string message = 
        "Invalid 'EXPORT' specification. Missing \"localDEF\" attribute";
    Console(LogLevel::Warning) << message << std::endl;
  }  else if ( as_name.empty() ) {
    string message = 
        "Invalid 'IMPORT' specification. Missing \"AS\" attribute";
    Console(LogLevel::Warning)<< message << std::endl;
  } else {
    // Lookup the nodes and fields and set up the route.
    Node *n = DEF_map->getNode( local_def_name);
    if( n ) {
      exported_nodes->addNode(as_name, n ); 
    } else {
      Console(LogLevel::Warning) << "Warning: EXPORT error. Node named \"" 
           << local_def_name << "\" does not exist."
           << endl;
    }
  }
  // push NULL on the node stack to skip elements within EXPORT element.
  //node_stack.push( NodeElement( NULL ) );
}
void X3DToBinary::handleCdataElement(NodeFieldWrap* pParent) {
  int numberOfValues = -1;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  std::string sValue;
  char fieldValueType;
  
  myfile.read((char *) &numberOfValues, sizeof(int));
  myfile.read(&valueType, sizeof(char));
  sValue = BUTILS::readString(myfile, numberOfValues);
  if( X3DUrlObject *url_object = 
    dynamic_cast< X3DUrlObject * >( pParent->pParent ) ) {
      url_object->url->push_back( sValue );
  }
}

void X3DToBinary::handleProgramSettingElement( unsigned int numAttribs ) {
  int attribNameSize = -1;
  std::string attribName;
  int numberOfValues = -1;
  char valueType = ' '; //Can be either 'i', 'd', 's'
  std::string sValue;
  int* iValues;
  double* dValues;
  char fieldValueType;

  std::string node_name, field_name, setting_name, setting_section;
  setting_name = ""; setting_section = "Main settings";
  
  for( unsigned int i = 0; i < numAttribs; ++i ) {
    
    myfile.read((char *) &attribNameSize, sizeof(int));
    attribName = BUTILS::readString(myfile, attribNameSize);
    myfile.read((char *) &numberOfValues, sizeof(int));
    myfile.read(&valueType, sizeof(char));

    switch(valueType) {
    case 'i':
      iValues = new int [numberOfValues];
      myfile.read((char*) iValues,  numberOfValues * sizeof(int));
     
      break;
      
    case 'd':
      dValues = new double [numberOfValues];
      myfile.read((char*) dValues,  numberOfValues * sizeof(double));
     
      break;
    case 's':
      sValue = BUTILS::readString(myfile, numberOfValues);
      break;
    default:
      break;
    }

    if( node_name.empty() && attribName == "node" ) {
      node_name = sValue;
    } else if( field_name.empty() &&  attribName == "field" ) {
      field_name = sValue;
    } else if( attribName == "name" ) {
      setting_name = sValue;
    } else if( attribName == "section" ) {
      setting_section = sValue;
    } else {
      Console(LogLevel::Warning) << "Warning: Unknown attribute \"" << attribName  
             << " in PROGRAM_SETTING element" << endl;
    }
  }
        
            
  // All route attributes were not found, so whrow exception
  if( node_name.empty() ) {
    std::cout << "Invalid PROGRAM_SETTING specification. Missing \"node\" attribute" << std::endl;
    
  } else if ( field_name.empty() ) {
    std::cout << "Invalid PROGRAM_SETTING specification. Missing \"field\" attribute" << std::endl;
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
        Console(LogLevel::Warning) << "Warning: Program setting error. Could not find field named \"" 
             << field_name
             << "\" in \"" << node_name << "\" Node " 
             << endl;
      }
    } else {
      Console(LogLevel::Warning) << "Warning: program setting error. Could not find Node named \"" 
           << node_name
           << "\" specified in \"node\" attribute " 
            << endl;
    }
  }

  // push NULL on the node stack to skip elements within PROGRAM_SETTING element.
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
    int nodeNameSize = -1;
    int numAttribs = -1;
    int numChildren = -1;
    int nodeID = -1;
    std::string nodeName;
    H3D::Node* pNewNode;
    myfile.read((char*)&nodeNameSize, sizeof(int));
    nodeName = BUTILS::readString(myfile, nodeNameSize);
     //myfile.read(nodeName, nodeNameSize);
    //nodeName[nodeNameSize] = '\0';
    myfile.read((char*) &nodeID, sizeof(int));
    myfile.read((char*) &numAttribs, sizeof(int));
    myfile.read((char*) &numChildren, sizeof(int));
    
    //TimeStamp startTNameCheck = TimeStamp::now();
    GlobalSettings *gs;
    bool event;
    switch (nodeID) {
    case 243: //Program_Setting
      handleProgramSettingElement( numAttribs );
      break;
    case 244: //ROUTE
      event = GlobalSettings::default_x3d_route_sends_event;
      gs = GlobalSettings::getActive();
      if( gs ) event = gs->x3dROUTESendsEvent->getValue();
      handleRouteElement(numAttribs, !event );
      break;
    case 245: //Route_No_Event
      handleRouteElement(numAttribs, true );
      break;
    case 246: //ROUTE_WITH_EVENT
      handleRouteElement(numAttribs, false );
      break;
    case 247: //IMPORT
      handleImportElement( numAttribs );
      break;
    case 248: //EXPORT
      handleExportElement( numAttribs );
      break;
    case 249: //field
      handleFieldElement(numAttribs,numChildren, pParent);
      break;
    case 250: //CDATA
      handleCdataElement(pParent);
      break;
    default:
      //TimeStamp endTNameCheck = TimeStamp::now();
     // timeACCNodeNameCheck = timeACCNodeNameCheck + endTNameCheck - startTNameCheck;
      try { 
        //TimeStamp startTDatabase = TimeStamp::now();
        if(nodeID == -1) {
          pNewNode = H3D::H3DNodeDatabase::createNode( nodeName );
        }
        else {
          pNewNode = NodePointers[nodeID]->createNode();
        }
        //TimeStamp endTDatabase = TimeStamp::now();
        //timeACCDatabase = timeACCDatabase + endTDatabase - startTDatabase;

      if( pNewNode ) {
              // remove the initialization that occurs on first reference
                // since we don't want it to occur until all child nodes
                // have been created and set.
                pNewNode->setManualInitialize( true );

                // if node is a script node add the current named nodes
                // from the current DEF_map and store the node in the 
                // script_nodes vector. When parsing is done the named
                // nodes will be updated in all scripts to contain
                // all named nodes from the parsed file.
                                
                if( H3DScriptNode *script_node = 
                    dynamic_cast< H3DScriptNode * >( pNewNode ) ) {
                  script_node->addNamedNodes( DEF_map );
                  script_nodes.push_back( script_node );
                } 
      } else {
        std::cout << "Warning: Could not create \"" << nodeName
          << "\" node. It does not exist in the H3DNod   eDatabase " << std::endl;
      }
      } catch (const H3D::Exception::H3DException &e) {
        std::cout << "Warning: Could not create \"" << nodeName 
          << "\" node. Exception in constructor: "
          << e  << std::endl;
    }

    std::string containerFieldName = pNewNode ? pNewNode->defaultXMLContainerField() : std::string("") ;
    useNode.USE = false;
    useNode.COPY= false;

    for(int i = 0; i < numAttribs; ++i) {
     // TimeStamp startTattr = TimeStamp::now();
      parseAttributesH3D(pNewNode, &containerFieldName);
      //TimeStamp endTAttr = TimeStamp::now();
      //timeACCattrib = timeACCattrib + (endTAttr-startTattr);
    }
   
    if( useNode.USE ) {
      // if we have a USE attribute, lookup the matching node and use that.
      pNewNode = DEF_map->getNode( useNode.nodeToUse);
      if(containerFieldName.empty()) 
        containerFieldName = pNewNode->defaultXMLContainerField();
      if ( useNode.COPY && pNewNode )
      {
        if ( !useNode.defName.empty() )
        {
          Node *copied_node = pNewNode->clone();
          DEF_map->addNode( useNode.defName, copied_node );
          copied_node->setName( useNode.defName );
          pNewNode = copied_node;
        }
        else
        {
          stringstream s;
          s << "Invalid COPY attribute. " 
            << "No DEF node name defined.";
        }
      }
      
      if( !pNewNode ) {
        stringstream s;
        s << "Invalid USE attribute. " 
          << "No node with DEF name \"" << useNode.defName 
          << "\" defined.";
      }
    }
   
    NodeFieldWrap* nfw = new NodeFieldWrap();
    nfw->pParent = pNewNode;
    nfw->isField = false;
    

   for (int i = 0; i < numChildren; ++i) 
      Node * pDump = parseNodesH3D(nfw, false);
    
    
   if( !pNewNode->isInitialized() && pNewNode->getManualInitialize() ) 
      pNewNode->initialize();

    //startTime = TimeStamp::now();
    if(pParent->isField) {
      Field::AccessType access_type = pParent->field->getAccessType();
        if( access_type == Field::INITIALIZE_ONLY ||
            access_type == Field::INPUT_OUTPUT ) {
          SFNode *sf = dynamic_cast< SFNode *>( pParent->field );
          if( sf ) { 
            sf->setValue( pNewNode );
          } else {
            MFNode *mf = dynamic_cast< MFNode *>( pParent->field );
            if( mf ) {
              mf->push_back( pNewNode );
            }
            else Console(LogLevel::Warning) << "Warning: 'value' element ignored. Only used if "
                      << "field type is SFNode or MFNode" 
                      << endl;
          }
        } else {
          Console(LogLevel::Warning) << "Warning: 'value' element ignored. Only used if "
               << "accesstype is initializeOnly or inputOutput " 
               << endl;
        }
    }
    else {
      H3D::Field *f = pParent->pParent->getField(containerFieldName);
      if( f ) {
        H3D::SFNode *sf = dynamic_cast< H3D::SFNode *>(f);
        if(sf) {
          sf->setValue(pNewNode);
        }
        else {
          H3D::MFNode *mf = dynamic_cast< H3D::MFNode *>(f);
          if( mf ) {
            mf->push_back( pNewNode );
          }
        }
      }
      else { // (!f) Could not get field
        std::cout << "Invalid containerField attribute \"" 
          <<  containerFieldName
          << "\". Field is not of type SFNode or MFNode.";
      }

    }
    //endTime = TimeStamp::now();
    //TimeStamp tSetField = endTime - startTime;
    }

    //TODO:Rustam: this has to be tested!
    if( nodeName == "Inline" ) {
      Inline *inline_node = dynamic_cast< Inline * >( pNewNode );

      if( inline_node ) {
        const string &import_mode = inline_node->importMode->getValue();      
        if( import_mode == "AUTO" || import_mode == "AUTO_IMPORT" ) {
          DEF_map->merge( &inline_node->exported_nodes );
        }
      }
    }
    
    if(!isRoot) 
    {
      return new Node();
    }
    else 
    {
      return pNewNode;
    }
    //std::cout << nodeNameSize << nodeName;
    //end = now()
    // map[nodeName] = end - start;
}
H3D::Node* X3DToBinary::readBinaryRoot(std::string fileToLoad){
  myfile.open(fileToLoad, std::ios::binary | std::fstream::in);
  int headerLength;

  if(myfile.is_open()){
    std::string fileVersion;
    myfile.read((char *) &headerLength, sizeof(int));
    fileVersion = BUTILS::readString(myfile, headerLength);
    if(fileVersion == VERSION) {
      NodeFieldWrap* nfw = new NodeFieldWrap();
      nfw->pParent = new Node();
      nfw->field = false;
      Node* rootNode = parseNodesH3D(nfw, true);
      myfile.close();
      return rootNode;
    } else {
      std::cout << "Could not load. File Version: * " << fileVersion << " * is old. Current Version: " << VERSION << std::endl; 
      return NULL;
    }
  }
  myfile.close();
  return NULL;

}
void X3DToBinary::setFilePath(const std::string& fileToLoad){
  filePath = fileToLoad;
}
void X3DToBinary::readBinary(std::string fileToLoad){

  myfile.open("D:\\Rustam\\testscenes\\example.txt", std::ios::binary | std::fstream::in);
  if(myfile.is_open()){
    //parseNodesH3D();
  }
  myfile.close();
}
X3DToBinary::X3DToBinary(X3D::DEFNodes * _def_map, X3D::DEFNodes * _exported_nodes) : VERSION("RA02"), DEF_map(_def_map), exported_nodes(_exported_nodes) {
  if(!DEF_map) {
    DEF_map = new X3D::DEFNodes();
  }
  if( !_exported_nodes ) {
    exported_nodes = new X3D::DEFNodes();
  }
  initialiseNodeDB();
}
}


