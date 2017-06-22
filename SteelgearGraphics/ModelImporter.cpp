#include "ModelImporter.h"

ModelImporter::ModelImporter(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, string filename, wchar_t directoryPath[200])
{
	wcscpy(directory, directoryPath);
	objName = filename;


	ifstream file;
	string temp;
	std::string::size_type sz;

	setCounters(filename); // Count positions, uv coords, normals and faces
	string mName = getMtlName(filename); // Get the mtl file name
	loadMTL(gDevice, gDeviceContext, mName); // Load the texture from the mtl file

	char data, last = NULL, failsafe = NULL;


	file.open(filename);
	file.get(data);
	while (!file.eof())
	{
		if (data == ' ' && last == 'v') // Read all position data
		{
			for (int i = 0; i < vCount; i++)
			{
				file.get(data);

				while (data != ' ')
				{
					temp += data;
					file.get(data);
				}


				vertices[i].x = stof(temp, &sz);
				temp = "";



				file.get(data);

				while (data != ' ')
				{
					temp += data;
					file.get(data);
				}

				vertices[i].y = stof(temp, &sz);
				temp = "";



				file.get(data);

				while (data != '\n')
				{
					temp += data;
					file.get(data);
				}

				vertices[i].z = stof(temp, &sz);
				temp = "";

				file.get(data);

				if (data == 'g')
					while (data != 'v')
					{
						file.get(data);
					}

				//while (data != 'v')
				//{
				//	file.get(data);
				//}

				file.get(data);


			}
		}

		//---------------------------------------------------------------------------------------------------------------------------------

		if (data == 't' && last == 'v') // Read all UV data
		{
			for (int i = 0; i < vtCount; i++)
			{
				file.get(data);
				file.get(data);

				while (data != ' ')
				{
					temp += data;
					file.get(data);
				}

				uvCoords[i].x = stof(temp, &sz);
				temp = "";



				file.get(data);

				while (data != '\n')
				{
					temp += data;
					file.get(data);
				}

				uvCoords[i].y = 1 - stof(temp, &sz);
				temp = "";

				file.get(data);
				file.get(data);


			}
		}

		//---------------------------------------------------------------------------------------------------------

		if (data == 'n' && last == 'v') // Read all Normals data
		{
			for (int i = 0; i < vnCount; i++)
			{
				file.get(data);
				file.get(data);

				while (data != ' ')
				{
					temp += data;
					file.get(data);
				}

				normals[i].x = stof(temp, &sz);
				temp = "";



				file.get(data);

				while (data != ' ')
				{
					temp += data;
					file.get(data);
				}

				normals[i].y = stof(temp, &sz);
				temp = "";



				file.get(data);

				while (data != '\n')
				{
					temp += data;
					file.get(data);
				}

				normals[i].z = stof(temp, &sz);
				temp = "";

				file.get(data);
				file.get(data);

			}
		}

		//---------------------------------------------------------------------------------------------------------------------------

		if (data == ' ' && last == 'f') // Read all face data and put everything together
		{
			int test = 0;
			int index = 0;

			for (int i = 0; i < iCount; i++)
			{
				int count;
				file.get(data);


				for (int j = 0; j < 3; j++)
				{

					while (data != '/')
					{
						temp += data;
						file.get(data);
					}

					file.get(data);
					index = stoi(temp, &sz);
					temp = "";

					count = iCount * 3 - 1 - i * 3 - j;

					modelData[count].x = vertices[index - 1].x;
					modelData[count].y = vertices[index - 1].y;
					modelData[count].z = vertices[index - 1].z;

					while (data != '/')
					{
						temp += data;
						file.get(data);
					}

					file.get(data);
					index = stoi(temp, &sz);
					temp = "";

					modelData[count].u = uvCoords[index - 1].x;
					modelData[count].v = uvCoords[index - 1].y;

					while (data != ' ' && data != '\n')
					{
						temp += data;
						file.get(data);
					}

					file.get(data);
					index = stoi(temp, &sz);
					temp = "";

					modelData[count].nx = normals[index - 1].x;
					modelData[count].ny = normals[index - 1].y;
					modelData[count].nz = normals[index - 1].z;

				}



				test++;

				while (data != 'f' && !file.eof() )
				{
					failsafe = data;
					file.get(data);

					if (data == 'f' && failsafe != '\n')
						while (!(data == 'f' && failsafe == '\n'))
						{
							failsafe = data;
							file.get(data);
						}

				}

				file.get(data);

			}
		}


		last = data;
		file.get(data);
	}

	setBuffers(gDevice);
}

ModelImporter::~ModelImporter()
{
	SafeReleaseD3D(modelVertexBuffer);
	SafeReleaseD3D(modelIndexBuffer);
	SafeReleaseD3D(texture);

	delete vertices;
	delete uvCoords;
	delete normals;
	delete indices;

	delete modelData;
}

void ModelImporter::setCounters(string filename) // Count the different data types
{
	ifstream file;

	vCount = vtCount = vnCount = iCount = 0;

	char data, last = NULL;

	file.open(filename);
	file.get(data);
	while (!file.eof())
	{
		if (data == ' ' && last == 'v')
			vCount++;

		if (data == 't' && last == 'v')
			vtCount++;

		if (data == 'n' && last == 'v')
			vnCount++;

		if (data == ' ' && last == 'f')
			iCount++;

		last = data;
		file.get(data);
	}
	file.close();

	vertices = new XMFLOAT3[vCount];
	uvCoords = new XMFLOAT2[vtCount];
	normals = new XMFLOAT3[vnCount];
	modelData = new ObjVertexData[iCount * 3];
}

void ModelImporter::setBuffers(ID3D11Device* gDevice)
{
	HRESULT test;

	D3D11_BUFFER_DESC vBufferDesc;
	memset(&vBufferDesc, 0, sizeof(vBufferDesc));
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(ObjVertexData) * iCount * 3;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = modelData;
	gDevice->CreateBuffer(&vBufferDesc, &vData, &modelVertexBuffer);

	indices = new UINT[iCount*3];

	for (int i = 0; i < iCount * 3; i++)
	{
		indices[i] = iCount*3 - 1 - i;
	}

	D3D11_BUFFER_DESC iBufferDesc;
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(int) * iCount * 3;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA orderData;
	orderData.pSysMem = indices;
	test = gDevice->CreateBuffer(&iBufferDesc, &orderData, &modelIndexBuffer);
}

ID3D11Buffer** ModelImporter::getVBuffer()
{
	return &modelVertexBuffer;
}

ID3D11Buffer* ModelImporter::getIBuffer()
{
	return modelIndexBuffer;
}

int ModelImporter::getNrOfIndices()
{
	return iCount * 3;
}

string ModelImporter::GetObjName()
{
	return objName;
}

void ModelImporter::loadMTL(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, string mtlname)
{
	HRESULT hr;
	wstring pathname(directory);
	wstring filepath(directory);
	pathname = pathname + L"\\Textures\\Objects\\";
	filepath = filepath + L"\\Obj\\";
	filepath.append(mtlname.begin(), mtlname.end());
	string test;
	test.assign(filepath.begin(), filepath.end());

	if (mtlname.compare("") == 0) // If no name was found, use default texture
	{
		CreateWICTextureFromFile(gDevice, gDeviceContext, L"dragonTex.jpg", 0, &texture, NULL);
		return;
	}

	ifstream file;
	string temps;
	wstring temp;

	std::string::size_type sz;

	char data, last = NULL, failsafe = NULL;


	file.open(test.c_str());

	if (file.fail()) // If file can't be opened, use default texture
	{
		CreateWICTextureFromFile(gDevice, gDeviceContext, L"dragonTex.jpg", 0, &texture, NULL);
		file.close();
		return;
	}

	file.get(data);
	while (!file.eof())
	{
			file.get(data);

			while (data != ' ' && data != '\n')
			{
				temps += data;
				file.get(data);
			}
			

			if (temps.compare("map_Kd") == 0)
			{
				temp = L"";
				file.get(data);

				while (data != ' ' && data != '\n')
				{
					temp += data;
					file.get(data);
				}

				temp = pathname + temp;
				CreateWICTextureFromFile(gDevice, gDeviceContext, temp.c_str(), 0, &texture, NULL);

				file.close();
				return;
			}
			temp = L"";
			temps = "";


		}

}

ID3D11ShaderResourceView** ModelImporter::getTexture()
{
	return &texture;
}

string ModelImporter::getMtlName(string filename)
{
	ifstream file;
	string temp;

	char data;

	file.open(filename);
	file.get(data);
	while (!file.eof())
	{

		while (data != ' ' && data != '\n')
		{
			temp += data;
			file.get(data);
		}


		if (temp.compare("mtllib") == 0)
		{
			temp = "";
			file.get(data);

			while (data != ' ' && data != '\n')
			{
				temp += data;
				file.get(data);
			}

			file.close();
			return temp;
		}

		temp = "";
		file.get(data);
	}
	file.close();

	return "";
}