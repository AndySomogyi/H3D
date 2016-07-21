#include <H3D/X3DToBinary.h>
using namespace H3D;
namespace BUTILS {
	bool icompare_pred(unsigned char a, unsigned char b)
	{
		return tolower(a) == tolower(b);
	}

	/// Returns true if 2 strings are equal case-sensitive.
	bool compare_case_sensitive(std::string const& a, std::string const& b)
	{
		if (a.length() == b.length()) {
			return std::equal(b.begin(), b.end(),
				a.begin(), icompare_pred);
		}
		else {
			return false;
		}
	}

	/// Removes trailing whitespaces from beginning and from the end of string
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

	/// Substitues multiple of character occurances with string to be filled.
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

	std::vector<double> getDoublesFromString(const std::string& str, const std::string delim = " ") {

		std::vector<double> values;
		values.reserve(str.size());
		auto strBegin = str.find_first_not_of(delim);
		while (strBegin != std::string::npos) {
			const auto strEnd = str.find_first_of(delim, strBegin + 1);
			auto strRange = strEnd - strBegin + 1;
			if (strEnd == std::string::npos) {
				strRange = str.size() - strBegin;
				values.push_back(std::stod(str.substr(strBegin, strRange)));
				break;
			}

			values.push_back(std::stod(str.substr(strBegin, strRange)));
			strBegin = str.find_first_not_of(delim, strEnd + 1);
		}
		return values;
	}

	/// Returns number of children of TiXmlNode
	int getNumberOfChildren(TiXmlNode* pParent) {
		TiXmlNode* pChild;
		int numOfChildren = 0;
		for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
		{
			if (pChild->Type() != TiXmlNode::TINYXML_COMMENT && pChild->Type() != TiXmlNode::TINYXML_UNKNOWN)
				numOfChildren++;
		}
		return numOfChildren;
	}
	/// Returns number of attributes of TiXmlElement
	int getNumberOfAttributes(TiXmlElement* pElement) {
		int numOfAttrib = 0;
		for (TiXmlAttribute* pAttrib = pElement->FirstAttribute(); pAttrib; pAttrib = pAttrib->Next()) {
			if (pAttrib->ValueStr().size() != 0)
				numOfAttrib++;
		}
		return numOfAttrib;
	}

	template<typename A, typename B>
	std::pair<B, A> flip_pair(const std::pair<A, B> &p)
	{
		return std::pair<B, A>(p.second, p.first);
	}

	template<typename A, typename B>
	std::multimap<B, A> flip_map(const std::map<A, B> &src)
	{
		std::multimap<B, A> dst;
		std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
			flip_pair<A, B>);
		return dst;
	}
	/// Reads a sequence of bytes in binary file and returns it as a string
	std::string readString(std::fstream &stream, unsigned int  num_bytes)
	{
		std::string result(num_bytes, ' ');
		stream.read(&result[0], num_bytes);

		return result;
	}

}

namespace H3D {
	void X3DToBinary::calculateOccurrences(TiXmlNode* p_parent) {
		std::map<std::string, unsigned int> ::iterator it;
		TiXmlNode* pChild;
		int t = p_parent->Type();
		switch (t) {
		case TiXmlNode::TINYXML_UNKNOWN:


		case TiXmlNode::TINYXML_TEXT:

		case TiXmlNode::TINYXML_DECLARATION:
			cout << "T is: " << p_parent->ValueStr();
		}
		if (t == TiXmlNode::TINYXML_ELEMENT) {
			it = Occurrences.find(p_parent->ValueStr());
			if (it == Occurrences.end())
				Occurrences[p_parent->ValueStr()] = 1;
			else
				it->second++;
			for (pChild = p_parent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
			{
				calculateOccurrences(pChild);
			}
		}
	}
	void X3DToBinary::printOccurrences(std::string file_to_load) {
		TiXmlDocument doc;
		doc.LoadFile(file_to_load);
		TiXmlElement* element = doc.RootElement();
		TiXmlNode* pParent = (TiXmlNode*)element;
		calculateOccurrences(pParent);
		multimap<unsigned int, std::string> map = BUTILS::flip_map(Occurrences);
		for (multimap<unsigned int, std::string>::iterator it = map.begin(); it != map.end(); ++it)
			std::cout << it->first << " => " << it->second << '\n';
	}
	/// Writes attributes to binary files then the parent node could not be identified. (slow to read, requires to parse string during reading)
	int X3DToBinary::writeAttributesToFile(TiXmlElement* p_element)
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
			unsigned int number_of_bytes = p_attrib->ValueStr().length();

			file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
			file_stream.write(p_attrib->Name(), length);
			file_stream.write(reinterpret_cast<const char *>(&number_of_bytes), sizeof(int));
			file_stream.write("u", sizeof(char));
			file_stream.write(p_attrib->Value(), number_of_bytes * sizeof(char));
			i++;
			p_attrib = p_attrib->Next();
		}
		return i;
	}
	/// Writes attributes to binary file. Has to be notified is it is writing '<field '
	int X3DToBinary::writeAttributesToFile(TiXmlElement* p_element, H3D::Node * p_parent, bool is_field)
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
			//if attrib is an empty string just move on to the next one
			if (p_attrib->ValueStr().length() == 0) {
				p_attrib = p_attrib->Next();
				continue;
			}

			int length = strlen(p_attrib->Name());
			// make sure that the current node is not a <field>. 
			if (!is_field)
				pField = p_parent->getField(p_attrib->Name());
			if (pField && !is_field) {
				// if Node's field is obtained correctly check what type it is supposed to be
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
						// just making sure number of elemtns in a string is greater than 0
						if (numberOfBytes == 0) {
							p_attrib = p_attrib->Next();
							continue;
						}
						file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
						file_stream.write(p_attrib->Name(), length);
						file_stream.write(reinterpret_cast<const char *>(&numberOfBytes), sizeof(int));
						file_stream.write("s", sizeof(char));
						file_stream.write(p_attrib->Value(), numberOfBytes * sizeof(char));

					}
					// DOUBLES if its not one of the above classes assume its double. (could be extendet in future to support floats, int ... 
					else
					{
						std::string str(p_attrib->Value());
						std::string chars_to_Clear(" \t\n\v\f\r,");
						std::vector<double> double_values = BUTILS::getDoublesFromString(str, chars_to_Clear);
						int numVal = double_values.size();
						file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
						file_stream.write(p_attrib->Name(), length);
						file_stream.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
						file_stream.write("d", sizeof(char));
						for (unsigned long long int i = 0; i < double_values.size(); i++) {
							double num = double_values[i];
							file_stream.write(reinterpret_cast<const char *>(&num), sizeof(double));
						}
					}
				}
			}
			//else if its a <field> then do our best to decide if attribute is double, integer or string
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

					file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
					file_stream.write(p_attrib->Name(), length);

					if (p_attrib->QueryDoubleValue(&d_val) == TIXML_SUCCESS) {

						int numVal = valuesD.size();
						file_stream.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
						file_stream.write("d", sizeof(char));
						for (unsigned long long int i = 0; i < valuesD.size(); i++) {
							double num = valuesD[i];
							file_stream.write(reinterpret_cast<const char *>(&num), sizeof(double));
						}
					}
					else {

						file_stream.write(reinterpret_cast<const char *>(&numVal), sizeof(int));
						file_stream.write("i", sizeof(char));
						for (unsigned long long int i = 0; i < valuesD.size(); i++) {
							double num = valuesD[i];
							file_stream.write(reinterpret_cast<const char *>(&num), sizeof(double));
						}
					}


				}
				else { // end if iteger 

					if (p_attrib->ValueStr().size() == 0) {
						p_attrib = p_attrib->Next();
						continue;
					}
					int length = strlen(p_attrib->Name());
					unsigned int number_of_bytes = p_attrib->ValueStr().length();

					file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
					file_stream.write(p_attrib->Name(), length);
					file_stream.write(reinterpret_cast<const char *>(&number_of_bytes), sizeof(int));
					file_stream.write("s", sizeof(char));
					file_stream.write(p_attrib->Value(), number_of_bytes * sizeof(char));


				}// end IF is double or int 
			}
			i++;
			p_attrib = p_attrib->Next();
		}
		return i;
	}

	/// Writes element directly as a string without cheking its contents
	void X3DToBinary::writeCDATAtoFile(TiXmlNode* p_parent) {
		int length = 5; // sizeof("cdata")
		int ID = 250;
		int num_atr = 1;
		int num_children = 0;
		unsigned int num_of_bytes = 0;
		file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
		file_stream.write("cdata", length * sizeof(char));

		file_stream.write(reinterpret_cast<const char *>(&ID), sizeof(int));
		file_stream.write(reinterpret_cast<const char *>(&num_atr), sizeof(int));
		file_stream.write(reinterpret_cast<const char *>(&num_children), sizeof(int));
		num_of_bytes = p_parent->ValueStr().size();
		file_stream.write(reinterpret_cast<const char *>(&num_of_bytes), sizeof(int));
		file_stream.write("s", sizeof(char));
		file_stream.write(p_parent->Value(), num_of_bytes * sizeof(char));
	}

	///Encodes TinyXMLs node to a binary file and writes it to file
	void X3DToBinary::writeNodeToFile(TiXmlNode* p_parent)
	{
		if (!p_parent) return;

		TiXmlNode* p_child;
		int t = p_parent->Type();

		//file_stream.write((char*) &tmpIndent, sizeof(tmpIndent));
		int num_of_children, length, num_of_attribs, num_of_bytes;
		int num;
		int ID;
		switch (t)
		{
		case TiXmlNode::TINYXML_DOCUMENT:
			Console(LogLevel::Error) << "TiXmlNode of type TINYXML_DOCUMENT is not supported in current version" << std::endl
				<< "Node name: " << p_parent->ValueStr() << " in file: " << current_file;
			break;

		case TiXmlNode::TINYXML_ELEMENT:
			length = p_parent->ValueStr().length(); //the length of the Node name
			num_of_attribs = BUTILS::getNumberOfAttributes(p_parent->ToElement());
			num_of_children = BUTILS::getNumberOfChildren(p_parent);

			file_stream.write(reinterpret_cast<const char *>(&length), sizeof(int));
			file_stream.write(p_parent->Value(), length * sizeof(char));

			//Here we are encoding most common nodes by assignin them IDs. These IDs' order is predefined in initialiseDB() function
			//ID is an integer at the moment. In future could be translated to CHAR due to low probability of having more than 256 IDs for nodes.
			ID = -1; //no such node in lookup table
			if (p_parent->ValueStr() == "Scene" || p_parent->ValueStr() == "Group") {
				ID = 0;
			}
			else if (p_parent->ValueStr() == "Transform") {
				ID = 1;
			}
			else if (p_parent->ValueStr() == "Shape") {
				ID = 2;
			}
			else if (p_parent->ValueStr() == "Coordinate") {
				ID = 3;
			}
			else if (p_parent->ValueStr() == "Appearance") {
				ID = 4;
			}
			else if (p_parent->ValueStr() == "Material") {
				ID = 5;
			}
			else if (p_parent->ValueStr() == "IndexedFaceSet") {
				ID = 6;
			}
			else if (p_parent->ValueStr() == "FrictionalSurface") {
				ID = 7;
			}
			else if (p_parent->ValueStr() == "ImageTexture") {
				ID = 8;
			}
			else if (p_parent->ValueStr() == "TextureTransform") {
				ID = 9;
			}
			else if (p_parent->ValueStr() == "TextureCoordinate") {
				ID = 10;
			}
			else if (p_parent->ValueStr() == "FitToBoxTransform") {
				ID = 11;
			}
			else if (p_parent->ValueStr() == "PointLight") {
				ID = 12;
			}
			else if (p_parent->ValueStr() == "DirectionalLight") {
				ID = 13;
			}
			else if (p_parent->ValueStr() == "ComposedShader") {
				ID = 14;
			}
			else if (p_parent->ValueStr() == "ShaderPart") {
				ID = 15;
			}
			else if (p_parent->ValueStr() == "PixelTexture") {
				ID = 16;
			}
			else if (p_parent->ValueStr() == "IndexedTriangleSet") {
				ID = 17;
			}
			else if (p_parent->ValueStr() == "Normal") {
				ID = 18;
			}
			else if (p_parent->ValueStr() == "FloatVertexAttribute") {
				ID = 19;
			}
			else if (p_parent->ValueStr() == "CoordinateInterpolator") {
				ID = 20;
			}
			else if (p_parent->ValueStr() == "NormalInterpolator") {
				ID = 21;
			}
			else if (p_parent->ValueStr() == "TimeSensor") {
				ID = 22;
			}
			else if (p_parent->ValueStr() == "SmoothSurface") {
				ID = 23;
			}
			else if (p_parent->ValueStr() == "ToggleGroup") {
				ID = 24;
			}
			else if (p_parent->ValueStr() == "field") {
				ID = 249;
			}
			else if (p_parent->ValueStr() == "EXPORT") {
				ID = 248;
			}
			else if (p_parent->ValueStr() == "IMPORT") {
				ID = 247;
			}
			else if (p_parent->ValueStr() == "ROUTE_WITH_EVENT") {
				ID = 246;
			}
			else if (p_parent->ValueStr() == "ROUTE_NO_EVENT") {
				ID = 245;
			}
			else if (p_parent->ValueStr() == "ROUTE") {
				ID = 244;
			}
			else if (p_parent->ValueStr() == "PROGRAM_SETTING") {
				ID = 243;
			}
			file_stream.write(reinterpret_cast<const char *>(&ID), sizeof(int));
			file_stream.write(reinterpret_cast<const char *>(&num_of_attribs), sizeof(int));
			file_stream.write(reinterpret_cast<const char *>(&num_of_children), sizeof(int));
			H3D::Node* pNewNode;

			//Make sure that our nod is not a scpecial node (field, route, ... ) Also additionally check that ID is within expected boundaries.
			if (ID < 243 && ID > -2) {
				//Try to create a Node. This is required to correctly identify the the attribute types that have to be written to the binary file.
				if (ID == -1) {
					pNewNode = H3D::H3DNodeDatabase::createNode(p_parent->ValueStr());
				}
				else {
					pNewNode = node_db_pointers[ID]->createNode();
				}
				//if node could be created or it is a field then 
				if (pNewNode) {
					//in case python script tries to initialize itself, which we do not want. Not sure if this line is required.
					pNewNode->setManualInitialize(true);
					num = writeAttributesToFile(p_parent->ToElement(), pNewNode, false);
				}
				// Else write attributes with unkown type. Then handle them during reading. -- SLOW!
				else
					num = writeAttributesToFile(p_parent->ToElement());
			}
			else {
				//Check if we are dealing with fields if yes
				if (ID == 249) //  == "field"
					num = writeAttributesToFile(p_parent->ToElement(), pNewNode, true);
				else
					num = writeAttributesToFile(p_parent->ToElement());
			}


		case TiXmlNode::TINYXML_COMMENT:
			break;

		case TiXmlNode::TINYXML_UNKNOWN:
			Console(LogLevel::Error) << "TiXmlNode of type TINYXML_UNKNOWN is not supported in current version" << std::endl
				<< "Node name: " << p_parent->ValueStr() << " in file: " << current_file;
			break;

		case TiXmlNode::TINYXML_TEXT:
			Console(LogLevel::Warning) << "TiXmlNode of type TINYXML_TEXT is assumed as CDATA" << std::endl
				<< "Node name: " << p_parent->ValueStr() << " in file: " << current_file;
			writeCDATAtoFile(p_parent);
			break;

		case TiXmlNode::TINYXML_DECLARATION:
			Console(LogLevel::Error) << "TiXmlNode of type TINYXML_DECLARATION is not supported in current version" << std::endl
				<< "Node name: " << p_parent->ValueStr() << " in file: " << current_file;
			break;
		default:
			break;
		}

		for (p_child = p_parent->FirstChild(); p_child != 0; p_child = p_child->NextSibling())
		{
			writeNodeToFile(p_child);
		}
		//file_stream << "\n" <<  ">" << pParent->ValueStr().length() << pParent->Value();
		//file_stream << "\n" <<  ">";
	}

	int X3DToBinary::writeToBinary(std::string file_to_load = "") {
		if (file_to_load.empty()) file_to_load = current_file;
		TiXmlDocument doc;
		doc.LoadFile(file_to_load);
		std::string pathToWrite = file_to_load + ".inprogress";
		std::remove(pathToWrite.c_str());
		std::remove((file_to_load + EXT).c_str());
		if (file_stream.is_open())
			file_stream.close();
		file_stream.open(pathToWrite, std::fstream::out | std::fstream::app | std::ios::binary);
		if (doc.ErrorId() == 0) {
			//put a flag at a very beginning to indicate if the file has been written successfuly

			std::string header = "" + VERSION;
			int header_length = header.size();
			file_stream.write(reinterpret_cast<const char*> (&header_length), sizeof(int));
			file_stream.write(header.c_str(), header_length * sizeof(char));

			TiXmlElement* element = doc.RootElement();
			TiXmlNode* p_parent = (TiXmlNode*)element;
			std::string name = p_parent->Value();
			// TODO:Rustam: Think over this code. How to find the root element for the binary file? Top element may be both <Scene> and not <scene> 
			bool root_found = false;
			//check if the root element is Scene or Group
			if (name != "Group" && name != "Scene") {
				//if root is Group or Scene try to find them in first level children
				for (TiXmlNode* pChild = element->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
				{
					std::string name = pChild->Value();
					if (name == "Group" || name == "Scene") {
						writeNodeToFile(pChild);
						root_found = true;
						break;
					}
				}
				//if could not find them just write the root element
				if (!root_found) {
					writeNodeToFile(p_parent);

				}
			}
			//if not group or scene and none of the children are, still write it
			else {
				writeNodeToFile(p_parent);
			}
		}
		else {
			file_stream.close();
			Console(LogLevel::Warning) << "H3DB: TinyXML could not load: " << file_to_load << std::endl;
			return 1;
		}

		/* file_stream.seekp(file_stream.beg);
		file_stream.write("<\"\"", 3 * sizeof(char));*/
		file_stream.close();
		//rename a file to appropriate filename to simbolize finish of writing
		if (rename(pathToWrite.c_str(), (file_to_load + EXT).c_str()) == 0)
			Console(LogLevel::Info) << "File successfully written and renamed. " << file_to_load + EXT << std::endl;
		else
			Console(LogLevel::Error) << "File could not be renamed: " << pathToWrite << std::endl;
		return 0;
	}

	template <typename T>
	/// Sets double, float, int values of a given ParsableField in efficient way
	void setFieldValue(H3D::ParsableField *pPField, unsigned int field_type, T *t, unsigned int num_of_elements) {

		switch (field_type) {
		case H3D::X3DTypes::SFFLOAT:
			if (H3D::SFFloat* f = static_cast<H3D::SFFloat*>(pPField)) {
				f->setValue(t[0]);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFDOUBLE:
			if (H3D::SFDouble* f = static_cast<H3D::SFDouble*>(pPField)) {
				f->setValue(t[0]);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFTIME:
			if (H3D::SFTime* f = static_cast<H3D::SFTime*>(pPField)) {
				f->setValue(t[0]);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFINT32:
			if (H3D::SFInt32* f = static_cast<H3D::SFInt32*>(pPField)) {
				f->setValue(t[0]);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFVEC2F:
			if (H3D::SFVec2f* f = static_cast<H3D::SFVec2f*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Vec2f(t[0], t[1]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFVEC2D:
			if (H3D::SFVec2d* f = static_cast<H3D::SFVec2d*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Vec2d(t[0], t[1]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFVEC3F:
			if (H3D::SFVec3f* f = static_cast<H3D::SFVec3f*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Vec3f(t[0], t[1], t[2]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFVEC3D:
			if (H3D::SFVec3d* f = static_cast<H3D::SFVec3d*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Vec3d(t[0], t[1], t[2]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFVEC4F:
			if (H3D::SFVec4f* f = static_cast<H3D::SFVec4f*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Vec4f(t[0], t[1], t[2], t[3]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFVEC4D:
			if (H3D::SFVec4d* f = static_cast<H3D::SFVec4d*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Vec4d(t[0], t[1], t[2], t[3]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;

		case H3D::X3DTypes::SFCOLOR:
			if (H3D::SFColor* f = static_cast<H3D::SFColor*>(pPField)) {
				f->setValue(RGB(t[0], t[1], t[2]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFCOLORRGBA:
			if (H3D::SFColorRGBA* f = static_cast<H3D::SFColorRGBA*>(pPField)) {
				f->setValue(RGBA(t[0], t[1], t[2], t[3]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFROTATION:
			if (H3D::SFRotation* f = static_cast<H3D::SFRotation*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Rotation(H3D::ArithmeticTypes::Vec3f(t[0], t[1], t[2]), t[3]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFQUATERNION:
			if (H3D::SFQuaternion* f = static_cast<H3D::SFQuaternion*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Quaternion(t[0], t[1], t[2], t[3]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFMATRIX3F:
			//TODO:Rustam: how are matrices given in X3D?
			if (H3D::SFMatrix3f* f = static_cast<H3D::SFMatrix3f*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Matrix3f(t[0], t[1], t[2],
					t[3], t[4], t[5],
					t[6], t[7], t[8]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFMATRIX4F:
			//TODO:Rustam: how are matrices given in X3D?
			if (H3D::SFMatrix4f* f = static_cast<H3D::SFMatrix4f*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Matrix4f(t[0], t[1], t[2], t[3],
					t[4], t[5], t[6], t[7],
					t[8], t[9], t[10], t[11],
					t[12], t[13], t[14], t[15]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFMATRIX3D:
			//TODO:Rustam: how are matrices given in X3D?
			if (H3D::SFMatrix3d* f = static_cast<H3D::SFMatrix3d*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Matrix3d(t[0], t[1], t[2],
					t[3], t[4], t[5],
					t[6], t[7], t[8]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFMATRIX4D:
			//TODO:Rustam: how are matrices given in X3D?
			if (H3D::SFMatrix4d* f = static_cast<H3D::SFMatrix4d*>(pPField)) {
				f->setValue(H3D::ArithmeticTypes::Matrix4d(t[0], t[1], t[2], t[3],
					t[4], t[5], t[6], t[7],
					t[8], t[9], t[10], t[11],
					t[12], t[13], t[14], t[15]));
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::SFIMAGE:
			//TODO:Rustam: what to do with images?!
			break;

			// =================== MF types =============================

		case H3D::X3DTypes::MFFLOAT:
			if (H3D::MFFloat* f = static_cast<H3D::MFFloat*>(pPField)) {
				std::vector<H3D::H3DFloat> vals;
				for (int i = 0; i < num_of_elements; i++) {
					vals.push_back(t[i]);
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFDOUBLE:
			if (H3D::MFDouble* f = static_cast<H3D::MFDouble*>(pPField)) {
				std::vector<H3D::H3DDouble> vals;
				for (int i = 0; i < num_of_elements; i++) {
					vals.push_back(t[i]);
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFTIME:
			if (H3D::MFTime* f = static_cast<H3D::MFTime*>(pPField)) {
				std::vector<H3D::H3DTime> vals;
				for (int i = 0; i < num_of_elements; i++) {
					vals.push_back(t[i]);
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFINT32:
			if (H3D::MFInt32* f = static_cast<H3D::MFInt32*>(pPField)) {
				std::vector<H3D::H3DInt32> vals;
				for (int i = 0; i < num_of_elements; i++) {
					vals.push_back(t[i]);
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFVEC2F:
			if (H3D::MFVec2f* f = static_cast<H3D::MFVec2f*>(pPField)) {
				std::vector<H3D::Vec2f> vals;
				for (int i = 0; i < num_of_elements; i += 2) {
					vals.push_back(H3D::Vec2f(t[i], t[i + 1]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFVEC2D:
			if (H3D::MFVec2d* f = static_cast<H3D::MFVec2d*>(pPField)) {
				std::vector<H3D::Vec2d> vals;
				for (int i = 0; i < num_of_elements; i += 2) {
					vals.push_back(H3D::Vec2d(t[i], t[i + 1]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFVEC3F:
			if (H3D::MFVec3f* f = static_cast<H3D::MFVec3f*>(pPField)) {
				std::vector<H3D::Vec3f> vals;
				for (int i = 0; i < num_of_elements; i += 3) {
					vals.push_back(H3D::Vec3f(t[i], t[i + 1], t[i + 2]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFVEC3D:
			if (H3D::MFVec3d* f = static_cast<H3D::MFVec3d*>(pPField)) {
				std::vector<H3D::Vec3d> vals;
				for (int i = 0; i < num_of_elements; i += 3) {
					vals.push_back(H3D::Vec3d(t[i], t[i + 1], t[i + 2]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFVEC4F:
			if (H3D::MFVec4f* f = static_cast<H3D::MFVec4f*>(pPField)) {
				std::vector<H3D::Vec4f> vals;
				for (int i = 0; i < num_of_elements; i += 4) {
					vals.push_back(H3D::Vec4f(t[i], t[i + 1], t[i + 2], t[i + 3]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFVEC4D:
			if (H3D::MFVec4d* f = static_cast<H3D::MFVec4d*>(pPField)) {
				std::vector<H3D::Vec4d> vals;
				for (int i = 0; i < num_of_elements; i += 4) {
					vals.push_back(H3D::Vec4d(t[i], t[i + 1], t[i + 2], t[i + 3]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFCOLOR:
			if (H3D::MFColor* f = static_cast<H3D::MFColor*>(pPField)) {
				std::vector<H3D::RGB> vals;
				for (int i = 0; i < num_of_elements; i += 3) {
					vals.push_back(H3D::RGB(t[i], t[i + 1], t[i + 2]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFCOLORRGBA:
			if (H3D::MFColorRGBA* f = static_cast<H3D::MFColorRGBA*>(pPField)) {
				std::vector<H3D::RGBA> vals;
				for (int i = 0; i < num_of_elements; i += 4) {
					vals.push_back(H3D::RGBA(t[i], t[i + 1], t[i + 2], t[i + 3]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFROTATION:
			if (H3D::MFRotation* f = static_cast<H3D::MFRotation*>(pPField)) {
				std::vector<H3D::Rotation> vals;
				for (int i = 0; i < num_of_elements; i += 4) {
					vals.push_back(H3D::Rotation(t[i], t[i + 1], t[i + 2], t[i + 3]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFQUATERNION:
			if (H3D::MFQuaternion* f = static_cast<H3D::MFQuaternion*>(pPField)) {
				std::vector<H3D::Quaternion> vals;
				for (int i = 0; i < num_of_elements; i += 4) {
					vals.push_back(H3D::Quaternion(t[i], t[i + 1], t[i + 2], t[i + 3]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFMATRIX3F:
			if (H3D::MFMatrix3f* f = static_cast<H3D::MFMatrix3f*>(pPField)) {
				std::vector<H3D::Matrix3f> vals;
				for (int i = 0; i < num_of_elements; i += 9) {
					vals.push_back(H3D::ArithmeticTypes::Matrix3f(t[0], t[1], t[2],
						t[3], t[4], t[5],
						t[6], t[7], t[8]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFMATRIX4F:
			if (H3D::MFMatrix4f* f = static_cast<H3D::MFMatrix4f*>(pPField)) {
				std::vector<H3D::Matrix4f> vals;
				for (int i = 0; i < num_of_elements; i += 16) {
					vals.push_back(H3D::ArithmeticTypes::Matrix4f(t[0], t[1], t[2], t[3],
						t[4], t[5], t[6], t[7],
						t[8], t[9], t[10], t[11],
						t[12], t[13], t[14], t[15]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFMATRIX3D:
			if (H3D::MFMatrix3d* f = static_cast<H3D::MFMatrix3d*>(pPField)) {
				std::vector<H3D::Matrix3d> vals;
				for (unsigned int i = 0; i < num_of_elements; i += 9) {
					vals.push_back(H3D::ArithmeticTypes::Matrix3d(t[0], t[1], t[2],
						t[3], t[4], t[5],
						t[6], t[7], t[8]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFMATRIX4D:
			if (H3D::MFMatrix4d* f = static_cast<H3D::MFMatrix4d*>(pPField)) {
				std::vector<H3D::Matrix4d> vals;
				for (unsigned int i = 0; i < num_of_elements; i += 16) {
					vals.push_back(H3D::ArithmeticTypes::Matrix4d(t[0], t[1], t[2], t[3],
						t[4], t[5], t[6], t[7],
						t[8], t[9], t[10], t[11],
						t[12], t[13], t[14], t[15]));
				}
				f->swap(vals);
			}
			else {
				std::cout << "Could not cast" << std::endl;
			}
			break;
		case H3D::X3DTypes::MFIMAGE:
			//TODO:Rustam: what tod o with IMAGES ? :(
			break;
		case H3D::X3DTypes::UNKNOWN_X3D_TYPE:
		default:
			std::cout << "Ooooops. Field type: " << field_type << " does not exist" << std::endl;
			break;

		} //END of switch(enumType)
	}
	/// Sets a string value of a given ParsableField in efficient way
	void setFieldValue(H3D::ParsableField *pPField, unsigned int field_type, std::string s_value) {
		bool test = false;
		switch (field_type) {
		case H3D::X3DTypes::SFBOOL:
			if (H3D::SFBool* f = static_cast<H3D::SFBool*>(pPField))
				f->setValue(BUTILS::compare_case_sensitive(s_value, "true"));
			break;

		case H3D::X3DTypes::SFSTRING:
			if (H3D::SFString* f = static_cast<H3D::SFString*>(pPField))
				f->setValue(s_value);
			break;
		case H3D::X3DTypes::MFBOOL:
			pPField->setValueFromString(s_value);
			break;
		case H3D::X3DTypes::MFSTRING:
			pPField->setValueFromString(s_value);
			break;
		case H3D::X3DTypes::UNKNOWN_X3D_TYPE:
		default:
			break;

		}
	}

	///Reads and handles attributes one at a time of a current node
	void X3DToBinary::parseAttributesH3D(H3D::Node* pNewNode, std::string* p_container_field_name) {
		Attribute attrib;
		readNextAttribute(attrib);
		if (attrib.attrib_name == "USE") {
			useNode.use_attrib = true;
			useNode.node_to_use = attrib.s_value;
		}
		else if (attrib.attrib_name == "COPY") {
			//TODO:Rusta: should it be casesensitive?
			if (attrib.s_value == "true")
				useNode.copy_attrib = true;

			// else it should be left FALSE
		}
		else if (attrib.attrib_name == "DEF") {
			if (pNewNode && DEF_map) {
				useNode.def_name = attrib.s_value;
				DEF_map->addNode(attrib.s_value, pNewNode);
				pNewNode->setName(attrib.s_value);

			}

		}
		else if (attrib.attrib_name == "class") {
			// class is a reserved attribute name 

		}
		else if (attrib.attrib_name == "containerField") {
			*p_container_field_name = attrib.s_value;
		}
		else {
			H3D::Field * pField = pNewNode->getField(attrib.attrib_name);

			//TODO:Rustam check this
			/* std::string typeName = pField->getTypeName();*/

			unsigned int fieldType;
			/// test->setValueFromString
			if (!pField) {
				std::cout << "Warning: Couldn't find field named \"" << attrib.attrib_name
					<< "\" in " << pNewNode->getName() << " node " << std::endl;
			}
			else {
				H3D::ParsableField *pPField = dynamic_cast< H3D::ParsableField * >(pField);
				if (!pPField) {
					std::cout << "Cannot parse field \"" << attrib.attrib_name
						<< "\". Field type must be a subclass of ParsableField "
						<< "in order to be parsable. ";
				}
				else {


					fieldType = pPField->getX3DType();
					switch (attrib.value_type) {
					case 'i':
						setFieldValue(pPField, fieldType, attrib.i_values, attrib.number_of_values);
						break;
					case 'd':
						setFieldValue(pPField, fieldType, attrib.d_values, attrib.number_of_values);
						break;
					case 's':
						/* s_value = readString(file_stream, number_of_values);*/
						setFieldValue(pPField, fieldType, attrib.s_value);
						break;
					case 'u':
						pPField->setValueFromString(attrib.s_value);

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
	///Reads and handles special element ROUTE from binary file
	void X3DToBinary::handleRouteElement(unsigned int num_of_attribs, bool route_no_event) {
		Attribute attrib;
		std::string from_node_name = "", to_node_name = "", from_field_name = "", to_field_name = "";
		for (int i = 0; i < num_of_attribs; ++i) {
			readNextAttribute(attrib);
			if (from_node_name.empty() && attrib.attrib_name == "fromNode") {
				from_node_name = attrib.s_value;
			}
			else if (to_node_name.empty() && attrib.attrib_name == "toNode") {
				to_node_name = attrib.s_value;
			}
			else if (from_field_name.empty() && attrib.attrib_name == "fromField") {
				from_field_name = attrib.s_value;
			}
			else if (to_field_name.empty() && attrib.attrib_name == "toField") {
				to_field_name = attrib.s_value;
			}
			else {
				Console(LogLevel::Warning) << "H3DB: Warning: Unknown attribute \"" << attrib.attrib_name
					<< "\" in ROUTE element " << endl;
			}
		}

		// All route attributes were not found, so whrow exception
		if (from_node_name.empty()) {
			string message = "Invalid ROUTE specification. Missing \"fromNode\" attribute ";
			Console(LogLevel::Error) << "H3DB: " << message << attrib.attrib_name
				<< "\" in ROUTE element " << endl;
		}
		else if (from_field_name.empty()) {
			string message = "Invalid ROUTE specification. Missing \"fromField\" attribute ";
			Console(LogLevel::Error) << "H3DB: " << message << attrib.attrib_name
				<< "\" in ROUTE element " << endl;
		}
		else if (to_node_name.empty()) {
			string message = "Invalid ROUTE specification. Missing \"toNode\" attribute ";
			Console(LogLevel::Error) << "H3DB: " << message << attrib.attrib_name
				<< "\" in ROUTE element " << endl;
		}
		else if (to_field_name.empty()) {
			string message = "Invalid ROUTE specification. Missing \"toField\" attribute ";
			Console(LogLevel::Error) << message << attrib.attrib_name
				<< "\" in ROUTE element " << endl;
		}
		else {
			// Lookup the nodes and fields and set up the route.
			Node *from_node = DEF_map->getNode(from_node_name);
			if (from_node) {
				Field *from_field =
					from_node->getField(from_field_name);
				if (from_field) {
					Node *to_node = DEF_map->getNode(to_node_name);
					if (to_node) {
						Field *to_field =
							to_node->getField(to_field_name);
						if (to_field) {
							if (!route_no_event)
								from_field->route(to_field);
							else
								from_field->routeNoEvent(to_field);
						}
						else {
							Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find field named \""
								<< to_field_name
								<< "\" in \""
								<< to_node_name
								<< "\" Node "
								<< current_file
								<< endl;
						}
					}
					else {
						Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find Node named \""
							<< to_node_name
							<< "\" specified in \"toNode\" attribute "
							<< current_file
							<< endl;
					}
				}
				else {
					Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find field named \""
						<< from_field_name
						<< "\" in \"" << from_node_name << "\" Node "
						<< current_file
						<< endl;
				}
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: Route error. Could not find Node named \""
					<< from_node_name
					<< "\" specified in \"fromNode\" attribute "
					<< current_file
					<< endl;
			}
		}
	}
	///Reads and handles special element FIELD from binary file
	void X3DToBinary::handleFieldElement(unsigned int num_of_attribs, unsigned int num_of_children, NodeFieldWrap* parent) {
		char field_value_type;
		double* d_values;
		std::string s_value;
		Attribute attrib;


		std::string field_name, field_type, field_access_type;
		bool has_field_value = false;

		for (unsigned int i = 0; i < num_of_attribs; ++i) {
			readNextAttribute(attrib);
			if (field_name.empty() && attrib.attrib_name == "name") {
				field_name = attrib.s_value;
			}
			else if (field_type.empty() && attrib.attrib_name == "type") {
				field_type = attrib.s_value;
			}
			else if (field_access_type.empty() && attrib.attrib_name == "accessType") {
				field_access_type = attrib.s_value;
			}
			else if (attrib.attrib_name == "value") {
				has_field_value = true;
				field_value_type = attrib.value_type;
				switch (field_value_type)
				{
				case 'd':
					//TODO:Rustam: this could be optimized by not using readNextAttribute() and directly obtaining value here. 
					//get read of one mem alloc.
					d_values = new double[attrib.number_of_values];
					std::copy(attrib.d_values, attrib.d_values + attrib.number_of_values, d_values);
					break;
				case 's':
				case 'u':
					s_value = attrib.s_value;
					break;
				default:
					break;
				}

			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
					<< "\" in 'field' element " << endl;
			}
		}
		H3DDynamicFieldsObject *dyn_object =
			dynamic_cast< H3DDynamicFieldsObject * >(parent->pParent);
		if (dyn_object) {
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
			Field *f = H3D::X3DTypes::newFieldInstance(field_type.c_str());

			if (!f) {
				string message =
					"Invalid value for 'type' attribute of 'field' specification.";
				Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
			}

			f->setOwner(parent->pParent);
			f->setName(field_name);

			string access_type_string = field_access_type;
			Field::AccessType access_type;
			if (access_type_string == "initializeOnly")
				access_type = Field::INITIALIZE_ONLY;
			else if (access_type_string == "outputOnly")
				access_type = Field::OUTPUT_ONLY;
			else if (access_type_string == "inputOnly")
				access_type = Field::INPUT_ONLY;
			else if (access_type_string == "inputOutput")
				access_type = Field::INPUT_OUTPUT;
			else {
				string message =
					"Invalid value for 'accessType' attribute of 'field' specification.";
				Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
			}

			if (has_field_value) {
				ParsableField *pfield =
					dynamic_cast< ParsableField * >(f);
				if (!pfield) {
					stringstream s;
					s << "Cannot parse field \"" << f->getFullName()
						<< "\". Field type must be a subclass of ParsableField "
						<< "in order to be parsable. ";
					Console(LogLevel::Warning) << "H3DB: " << s.str() << std::endl;
				}
				if (access_type == Field::INITIALIZE_ONLY ||
					access_type == Field::INPUT_OUTPUT ||
					access_type == Field::INPUT_ONLY) {

					unsigned int field_type_id = pfield->getX3DType();
					switch (field_value_type) {
					case 'i':
						setFieldValue(pfield, field_type_id, attrib.i_values, attrib.number_of_values);
						break;
					case 'd':
						setFieldValue(pfield, field_type_id, d_values, attrib.number_of_values);
						delete[] d_values;
						break;
					case 's':
						setFieldValue(pfield, field_type_id, s_value);
					case 'u':
						pfield->setValueFromString(s_value);
						break;
					}

				}
				else {
					Console(LogLevel::Warning) << "H3DB: " << "Warning: 'value' attribute ignored. Only used if "
						<< "accesstype is initializeOnly, inputOnly or inputOutput "
						<< endl;
				}
			}
			dyn_object->addField(field_name,
				access_type,
				f);
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
		}
		else {
			Console(LogLevel::Warning) << "H3DB: " << "Warning: 'field' declaration in Node that is does not support"
				<< " it " << endl;
			//node_stack.push( NodeElement( NULL ) ); 
		}

	}
	///Reads and handles special element IMPORT from binary file
	void X3DToBinary::handleImportElement(unsigned int num_of_attribs) {


		char fieldValueType;
		std::string inline_def_name, exported_def_name, as_name;
		Attribute attrib;
		for (unsigned int i = 0; i < num_of_attribs; ++i) {
			readNextAttribute(attrib);
			if (inline_def_name.empty() && attrib.attrib_name == "inlineDEF") {
				inline_def_name = attrib.s_value;
			}
			else if (exported_def_name.empty() && attrib.attrib_name == "exportedDEF") {
				exported_def_name = attrib.s_value;
			}
			else if (as_name.empty() && attrib.attrib_name == "AS") {
				as_name = attrib.s_value;
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
					<< "\" in 'field' element " << endl;
			}
		}

		if (inline_def_name.empty()) {
			string message =
				"Invalid 'field' specification. Missing \"name\" attribute";
			Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
		}
		else if (exported_def_name.empty()) {
			string message =
				"Invalid 'field' specification. Missing \"type\" attribute";
			Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
		}
		else if (as_name.empty()) {
			string message =
				"Invalid 'field' specification. Missing \"accessType\" attribute";
			Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
		}
		else {

			const std::string &s = exported_def_name;
			if (inline_def_name == "H3D_EXPORTS") {
				Node *import_node =
					H3D::H3DExports::getH3DExportNode(s);
				if (import_node) {
					DEF_map->addNode(as_name,
						import_node);
				}
				else {
					Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. H3D_EXPORTS "
						<< "does not include \""
						<< exported_def_name << "\""
						<< endl;
				}
			}
			else {
				Node *n = DEF_map->getNode(inline_def_name);
				Inline *inline_node = dynamic_cast< Inline * >(n);
				if (inline_node) {
					Node *import_node =
						inline_node->exported_nodes.getNode(exported_def_name);
					if (import_node) {
						DEF_map->addNode(as_name,
							import_node);
					}
					else {
						Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. Inline node \""
							<< inline_def_name << "\" does not EXPORT \""
							<< exported_def_name << "\""
							<< endl;
					}
				}
				else {
					if (n) {
						Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. Node \""
							<< inline_def_name << "\" is not an Inline node "
							<< endl;
					}
					else {
						Console(LogLevel::Warning) << "H3DB: " << "Warning: IMPORT error. Node named \""
							<< inline_def_name << "\" does not exist."
							<< endl;
					}
				}
			}

		}
	}
	///Reads and handles special element EXPORT from binary file
	void X3DToBinary::handleExportElement(unsigned int numAttribs) {
		char fieldValueType;

		std::string local_def_name, as_name;
		Attribute attrib;
		for (unsigned int i = 0; i < numAttribs; ++i) {
			readNextAttribute(attrib);
			if (local_def_name.empty() && attrib.attrib_name == "localDEF") {
				local_def_name = attrib.s_value;
			}
			else if (as_name.empty() && attrib.attrib_name == "AS") {
				as_name = attrib.s_value;
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
					<< " in EXPORT element" << endl;
			}
		}
		if (local_def_name.empty()) {
			string message =
				"Invalid 'EXPORT' specification. Missing \"localDEF\" attribute";
			Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
		}
		else if (as_name.empty()) {
			string message =
				"Invalid 'IMPORT' specification. Missing \"AS\" attribute";
			Console(LogLevel::Warning) << "H3DB: " << message << std::endl;
		}
		else {
			// Lookup the nodes and fields and set up the route.
			Node *n = DEF_map->getNode(local_def_name);
			if (n) {
				exported_nodes->addNode(as_name, n);
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: EXPORT error. Node named \""
					<< local_def_name << "\" does not exist."
					<< endl;
			}
		}
		// push NULL on the node stack to skip elements within EXPORT element.
		//node_stack.push( NodeElement( NULL ) );
	}
	///Reads  and handles special element CDATA from binary file 
	void X3DToBinary::handleCdataElement(NodeFieldWrap* pParent) {
		int num_of_values = -1;
		char value_type = ' '; //Can be only 's' for CDATA
		std::string s_value;

		file_stream.read((char *)&num_of_values, sizeof(int));
		file_stream.read(&value_type, sizeof(char));
		s_value = BUTILS::readString(file_stream, num_of_values);
		if (X3DUrlObject *url_object =
			dynamic_cast< X3DUrlObject * >(pParent->pParent)) {
			url_object->url->push_back(s_value);
		}
	}
	///Reads  and handles special element ProgramSetting from binary file
	void X3DToBinary::handleProgramSettingElement(unsigned int num_of_attribs) {
		Attribute attrib;
		std::string node_name, field_name, setting_name, setting_section;
		setting_name = ""; setting_section = "Main settings";

		for (unsigned int i = 0; i < num_of_attribs; ++i) {
			readNextAttribute(attrib);

			if (node_name.empty() && attrib.attrib_name == "node") {
				node_name = attrib.s_value;
			}
			else if (field_name.empty() && attrib.attrib_name == "field") {
				field_name = attrib.s_value;
			}
			else if (attrib.attrib_name == "name") {
				setting_name = attrib.s_value;
			}
			else if (attrib.attrib_name == "section") {
				setting_section = attrib.s_value;
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: Unknown attribute \"" << attrib.attrib_name
					<< " in PROGRAM_SETTING element" << endl;
			}
		}


		// All route attributes were not found, so whrow exception
		if (node_name.empty()) {
			Console(LogLevel::Warning) << "H3DB: " << "Invalid PROGRAM_SETTING specification. Missing \"node\" attribute" << std::endl;

		}
		else if (field_name.empty()) {
			Console(LogLevel::Warning) << "H3DB: " << "Invalid PROGRAM_SETTING specification. Missing \"field\" attribute" << std::endl;
		}
		else {
			// Lookup the nodes and fields and set up the route.
			Node *node = DEF_map->getNode(node_name);
			if (node) {
				Field *field =
					node->getField(field_name.c_str());
				if (field) {
					Scene::addProgramSetting(field,
						setting_name,
						setting_section);

				}
				else {
					Console(LogLevel::Warning) << "H3DB: " << "Warning: Program setting error. Could not find field named \""
						<< field_name
						<< "\" in \"" << node_name << "\" Node "
						<< endl;
				}
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Warning: program setting error. Could not find Node named \""
					<< node_name
					<< "\" specified in \"node\" attribute "
					<< endl;
			}
		}

		// push NULL on the node stack to skip elements within PROGRAM_SETTING element.
	}

	///Reads next attribute and stores it in Attribute struct
	void X3DToBinary::readNextAttribute(Attribute &a)
	{
		file_stream.read((char *)&a.attribute_name_length, sizeof(int));
		a.attrib_name = BUTILS::readString(file_stream, a.attribute_name_length);
		file_stream.read((char *)&a.number_of_values, sizeof(int));
		file_stream.read(&a.value_type, sizeof(char));

		switch (a.value_type) {
		case 'i':
			a.i_values = new int[a.number_of_values];
			file_stream.read((char*)a.i_values, a.number_of_values * sizeof(int));

			break;

		case 'd':
			a.d_values = new double[a.number_of_values];
			file_stream.read((char*)a.d_values, a.number_of_values * sizeof(double));

			break;
		case 's':
		case 'u':
			a.s_value = BUTILS::readString(file_stream, a.number_of_values);
			break;
		default:
			break;
		}
	}

	///Initializes a vector which represents pointers to most commonly used nodes' database
	///Order is important! Do not change the order of entries. Only push_back is allowed without breaking the code.
	///If the order has to be changed following functions have to be updated as well:
	///parseNodeH3D() and writeNodeToFile()
	void X3DToBinary::initialiseNodeDB() {
		//Order is important corresponds to our encoding.
		node_db_pointers.push_back(&Group::database);
		node_db_pointers.push_back(&Transform::database);
		node_db_pointers.push_back(&Shape::database);
		node_db_pointers.push_back(&Coordinate::database);
		node_db_pointers.push_back(&Appearance::database);
		node_db_pointers.push_back(&Material::database);
		node_db_pointers.push_back(&IndexedFaceSet::database);
		node_db_pointers.push_back(&FrictionalSurface::database);
		node_db_pointers.push_back(&ImageTexture::database);
		node_db_pointers.push_back(&TextureTransform::database);
		node_db_pointers.push_back(&TextureCoordinate::database);
		node_db_pointers.push_back(&FitToBoxTransform::database);

		node_db_pointers.push_back(&PointLight::database);
		node_db_pointers.push_back(&DirectionalLight::database);
		node_db_pointers.push_back(&ComposedShader::database);
		node_db_pointers.push_back(&ShaderPart::database);
		node_db_pointers.push_back(&PixelTexture::database);
		node_db_pointers.push_back(&IndexedTriangleSet::database);

		node_db_pointers.push_back(&Normal::database);
		node_db_pointers.push_back(&FloatVertexAttribute::database);
		node_db_pointers.push_back(&CoordinateInterpolator::database);
		node_db_pointers.push_back(&NormalInterpolator::database);

		node_db_pointers.push_back(&TimeSensor::database);
		node_db_pointers.push_back(&SmoothSurface::database);
		node_db_pointers.push_back(&ToggleGroup::database);
	}

	/// Reads and handles Node from a binary file
	H3D::Node* X3DToBinary::parseNodesH3D(NodeFieldWrap* pParent, bool isRoot) {
		// start = now()
		int node_name_length = -1;
		int num_of_attribs = -1;
		int num_of_children = -1;
		int node_id = -1;
		std::string node_name;
		H3D::Node* pNewNode;
		file_stream.read((char*)&node_name_length, sizeof(int));
		node_name = BUTILS::readString(file_stream, node_name_length);
		//file_stream.read(nodeName, nodeNameSize);
		//nodeName[nodeNameSize] = '\0';
		file_stream.read((char*)&node_id, sizeof(int));
		file_stream.read((char*)&num_of_attribs, sizeof(int));
		file_stream.read((char*)&num_of_children, sizeof(int));

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
					pNewNode = node_db_pointers[node_id]->createNode();
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

	///Initializes parsing of the binary file. Returns the root Node of the scene
	H3D::Node* X3DToBinary::readBinaryRoot(std::string file_to_load) {
		//checks if the file is open to read
		if (file_stream.is_open()) {
			int header_length;
			std::string file_version;
			file_stream.read((char *)&header_length, sizeof(int));
			file_version = BUTILS::readString(file_stream, header_length);
			if (file_version == VERSION) {
				NodeFieldWrap* pNFW = new NodeFieldWrap();
				pNFW->pParent = new Node();
				pNFW->field = false;
				Node* rootNode = parseNodesH3D(pNFW, true);
				file_stream.close();
				return rootNode;
			}
			else {
				Console(LogLevel::Warning) << "H3DB: " << "Could not load. File Version: * " << file_version << " * is old. Current Version: " << VERSION << std::endl;
				return NULL;
			}
		}
		Console(LogLevel::Error) << "H3DB: " << "Could not load BINARY file: " << file_to_load << EXT << std::endl;
		return NULL;

	}
	/// Sets the internal file path. Mostly used for debugging
	void X3DToBinary::setFilePath(const std::string& file_to_load) {
		current_file = file_to_load;
	}

	///Returns true if the binary file associated with the provided X3D file could be opened.
	bool X3DToBinary::openToRead(std::string &file_path) {
		/*std::string base = ResourceResolver::getBaseURL();
		if (base != "objects\tools" && base != "x3d\eyes\components" && base != "x3d/scenes/../eyes/components/")
		{
		Console(LogLevel::Warning) << "H3DB: " << "Omitting files in: " << base << std::endl;
		return false;
		}*/
		//Console(LogLevel::Warning) << "H3DB: " << "Opened files in: " << base << std::endl;
		std::string binary_file_path = file_path + EXT;
		if (!isX3DModified(file_path)) {
			file_stream.open(binary_file_path, std::ios::binary | std::ifstream::in);
			return file_stream.is_open();
		}
		return false;
	}

	///Returns true if the timestamp of X3D file is newer than the one of H3DB file associated with it 
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

	///Constructor required to specify file version to avoid confusion between reading and writing. VERSION will be cheked in readBinaryRoot() funciton
	X3DToBinary::X3DToBinary(X3D::DEFNodes * _def_map, X3D::DEFNodes * _exported_nodes) : VERSION("H3DB01"), EXT(".h3db"), DEF_map(_def_map), exported_nodes(_exported_nodes) {
		//DEF_map can be transfered between different X3D files using exported_nodes
		if (!DEF_map) {
			DEF_map = new X3D::DEFNodes();
		}
		if (!_exported_nodes) {
			exported_nodes = new X3D::DEFNodes();
		}
		initialiseNodeDB();
	}
}


