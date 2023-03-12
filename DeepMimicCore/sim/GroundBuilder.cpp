#include "GroundBuilder.h"
#include "sim/GroundPlane.h"
#include <iostream>
#include <fstream>

void ReadDataFromFileLBLIntoCharArray(const std::string& param_file)
{
	std::ifstream fin(param_file);
	//std::ifstream fin("data/terrain/plane.txt");//dllģʽ�� ok ../data ��ok
	if (!fin) {
		std::cout << "fail to open the file:" << param_file << std::endl;
	}
	const int LINE_LENGTH = 100; //һ:fstream.getline�ĵڶ���������Ҫ�����ַ����������ֽ������ĵ���û����ȷ˵����
	//��:������г����˻��壬��ѭ���������
	//�ܽ᣺��getline��ʱ��һ��Ҫ��֤�����������ܹ����ɸ��ֿ��ܵ������С��мǴ����ַ�����
	//�ڴ�������Ϊ����"data.txt"��ʱ��ÿһ�е��ַ�����Ҫ����100������whileѭ���������
	char str[LINE_LENGTH];
	while (fin.getline(str, LINE_LENGTH))
	{
		std::cout << "Read from file: " << str << std::endl;
	}
}

bool cGroundBuilder::ParseParamsJson(const std::string& param_file, cGround::tParams& out_params)
{
	std::ifstream f_stream(param_file);
	Json::Reader reader;
	Json::Value root;
	bool succ = reader.parse(f_stream, root);
	ReadDataFromFileLBLIntoCharArray(param_file);
	//std::cout << "c++ ParseParamsJson terrain_file:" << f_stream << std::endl;
	f_stream.close();

	if (succ)
	{
		if (!root[cGround::gTypeKey].isNull())
		{
			std::string type_str = root[cGround::gTypeKey].asString();
			cGround::ParseType(type_str, out_params.mType);
		}

		out_params.mGroundWidth = root.get(cGround::gGroundWidthKey, out_params.mGroundWidth).asDouble();
		out_params.mVertSpacingX = root.get(cGround::gVertSpacingXKey, out_params.mVertSpacingX).asDouble();
		out_params.mVertSpacingZ = root.get(cGround::gVertSpacingZKey, out_params.mVertSpacingZ).asDouble();
		
		if (!root[cGround::gParamsKey].isNull())
		{
			Json::Value params_arr = root[cGround::gParamsKey];
			assert(params_arr.isArray());
			int num = params_arr.size();

			out_params.mParamArr.resize(0, 0);
			if (num > 0)
			{
				for (int i = 0; i < num; ++i)
				{
					Eigen::VectorXd curr_params;
					succ &= ParseParamsJason(out_params.mType, params_arr.get(i, 0), curr_params);

					if (succ)
					{
						if (i == 0)
						{
							size_t m = num;
							size_t n = curr_params.size();
							out_params.mParamArr.resize(m, n);
						}
						out_params.mParamArr.row(i) = curr_params;
					}
				}
			}
		}
	}

	return succ;
}

void cGroundBuilder::BuildGround(const std::shared_ptr<cWorld>& world, const cGround::tParams& params, std::shared_ptr<cGround>& out_ground)
{
	cGround::eClass ground_class = cGround::GetClassFromType(params.mType);
	switch (ground_class)
	{
	case cGround::eClassPlane:
		BuildGroundPlane(world, params, out_ground);
		break;
	default:
		assert(false); // unsupported ground class
		break;
	}
}

bool cGroundBuilder::ParseParamsJason(cGround::eType type, const Json::Value& json, Eigen::VectorXd& out_params)
{
	cGround::eClass ground_class = cGround::GetClassFromType(type);
	bool succ = false;
	switch (ground_class)
	{
	case cGround::eClassPlane:
		succ = cGroundPlane::ParseParamsJson(json, out_params);
		break;
	default:
		assert(false); // unsupported ground class
		break;
	}
	return succ;
}

void cGroundBuilder::BuildGroundPlane(const std::shared_ptr<cWorld>& world, const cGround::tParams& params, std::shared_ptr<cGround>& out_ground)
{
	std::shared_ptr<cGroundPlane> ground_plane = std::shared_ptr<cGroundPlane>(new cGroundPlane());
	ground_plane->Init(world, params);
	out_ground = ground_plane;
}