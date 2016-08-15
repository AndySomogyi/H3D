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


	/*A class that handles writing and reading from binary x3d file format
	The file format is as following:
	The header is omitted. All binary files start with <Scene> 
	For example:
	<?xml version="1.0" encoding="utf-8"?>
	<X3D profile='Full' version='3.2'>
	  <head>
	    <meta name='title' content='Box.x3d'/>
	    <meta name='description' content='X3D Box node example.'/>
	    <meta name='author' content='SenseGraphics AB, 2006-2014'/>
	   </head>
	  <Scene>
	    <Viewpoint position='0   0    0.6666666666666666    ' />
	    <Shape DEF="myshape">
	      <Box size='0.2 0.1 0.05' solid='true' />
	    </Shape>
	  </Scene>
	</X3D>

	//TODO:Rustam How many bytes each one. Mention that Node name is not required. only for debugging. Check if id is unknown then read nodename.Write extensive documentation to what files are used and how overall works
	//write comments in .h file.
	Will be stored as ===== >
	  5Scene0 0 1 --------------------------------------------- [NnumberOfBytesInName][NodeName][ID][NumberOfAttributess][NumberOfChildren]
	    9Viewpoint-1 1 0 8position3d 0 0 0.666666 -------------- [NnumberOfBytesInName][NodeName][ID][NumberOfAttributess][NumberOfChildren][NumberOfBytesInAttribName][AttribName][NumberOfValues][ValuesType][double][double][double]
		5Shape2 1 2 3DEF7s myshape ---------------------------- [NnumberOfBytesInName][NodeName][ID][NumberOfAttributess][NumberOfChildren][NumberOfBytesInAttribName][AttribName][NumberOfValues][ValuesType][char]...[char]
		  3Box-1 2 0 4size3d 0.2 0.1 0.05 5solid4s true ------- [NnumberOfBytesInName][NodeName][ID][NumberOfAttributess][NumberOfChildren][NumberOfBytesInAttribName][AttribName][NumberOfValues][ValuesType][double][double][double][NumberOfBytesInAttribName][AttribName][NumberOfValues][ValuesType][char]...[char]

	Ignore all the spaces and identation. they are here only for readability

	*/
	class X3DToBinary {
	private:

		//TiXmlDocument doc;
		std::string current_file;
		H3D::X3D::DEFNodes* DEF_map;
		H3D::X3D::DEFNodes* exported_nodes;

		const std::string VERSION;
		const std::string EXT;

		std::map<std::string, unsigned int> Occurrences;

		std::fstream file_stream;

		AutoRefVector< H3DScriptNode > script_nodes;

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
			double* d_values;
			~Attribute() {
				switch (value_type)
				{
				case 'i':
					delete[] i_values;
					break;
				case 'd':
					delete[] d_values;
					break;
				default:
					break;
				}


			}
		};

		///Encodes TinyXMLs node to a binary file and writes it to file
		void writeNodeToFile(TiXmlNode* p_parent);
		/// Writes attributes to binary files then the parent node could not be identified. (slow to read, requires to parse string during reading)
		int writeAttributesToFile(TiXmlElement* p_element);
		/// Writes attributes to binary file. Has to be notified is it is writing '<field'
		int writeAttributesToFile(TiXmlElement* p_element, H3D::Node * pParent, bool is_field);
		/// Writes element directly as a string without cheking its contents
		void writeCDATAtoFile(TiXmlNode* p_parent);
		///Initializes a vector which represents pointers to most commonly used nodes' database
		///Order is important! Do not change the order of entries. Only push_back is allowed without breaking the code.
		///If the order has to be changed following functions have to be updated as well:
		///parseNodeH3D() and writeNodeToFile()
		void initialiseNodeDB();

		/// Reads and handles Node from a binary file
		H3D::Node* parseNodesH3D(NodeFieldWrap * pParent, bool isRoot);
		///Reads and handles attributes one at a time of a current node
		void parseAttributesH3D(H3D::Node * pNewNode, std::string * p_container_field_name);
		///Reads next attribute and stores it in Attribute struct
		void readNextAttribute(Attribute & a);
		///Reads and handles special element ROUTE from binary file
		void handleRouteElement(unsigned int num_of_attribs, bool route_no_event);
		///Reads and handles special element FIELD from binary file
		void handleFieldElement(unsigned int num_of_attribs, unsigned int num_of_children, NodeFieldWrap* pParent);
		///Reads and handles special element IMPORT from binary file
		void handleImportElement(unsigned int num_of_attribs);
		///Reads and handles special element EXPORT from binary file
		void handleExportElement(unsigned int num_of_attribs);
		///Reads  and handles special element CDATA from binary file 
		void handleCdataElement(NodeFieldWrap* parent);
		///Reads  and handles special element ProgramSetting from binary file
		void handleProgramSettingElement(unsigned int num_of_attribs);



		void calculateOccurrences(TiXmlNode* pParent);


	public:
		///Constructor required to specify file version to avoid confusion between reading and writing. VERSION will be cheked in readBinaryRoot() funciton
		X3DToBinary(X3D::DEFNodes * _def_map, X3D::DEFNodes * _exported_nodes);
		~X3DToBinary();
		///Returns true if the binary file associated with the provided X3D file could be opened.
		bool openToRead(std::string &file_path);
		///Returns true if the timestamp of X3D file is newer than the one of H3DB file associated with it 
		bool isX3DModified(std::string &file_path);
		/// Sets the internal file path. Mostly used for debugging
		void setFilePath(const std::string& file_to_load);
		int writeToBinary(std::string file_to_load);
		///Initializes parsing of the binary file. Returns the root Node of the scene
		H3D::Node* readBinaryRoot(std::string file_to_load);
		///Prints to the terminal frequencies of each nodes appearing in the given 3XD file. Useful to tweak initialiseNodeDB() to
		/// include the most relevant entries for the givein project.
		void printOccurrences(std::string file_to_load);
	};

}


#endif