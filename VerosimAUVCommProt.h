#ifndef __VEROSIM_AUV_COMM_PROT__H__
#define __VEROSIM_AUV_COMM_PROT__H__

// VIM - Verosim Interface Module

#define VEROSIM_AUV_INTERFACE_STATE_CMDID				11
#define VEROSIM_AUV_INTERFACE_DES_HDG_CMN_CMDID		22
#define VEROSIM_AUV_INTERFACE_DES_ARC_CMN_CMDID		33
#define VEROSIM_AUV_INTERFACE_DES_LINE_CMN_CMDID	44

#include <sstream>
#include <string>
#include <stdlib.h>

namespace VerosimAUVInterfaceModule
{
class VerosimAUVInterfaceModuleObject
{
	// additional
public:
	typedef enum 
	{
		Invalid = 0,
		State,
		PathSegment,
		AutopilotCommand
	} Type;

	// construction 
public:
	VerosimAUVInterfaceModuleObject(Type type){type = type;};
	virtual ~VerosimAUVInterfaceModuleObject(){};

	// services
public:
	virtual std::string toString(){return "";};
	Type getType(){return type;};

	// management
protected:
	virtual void initializeFromString(std::string str){};

	// data
protected:
	Type type;
}; // end VerosimAUVInterfaceModuleHandler

class VerosimAUVInterfaceModuleState : public VerosimAUVInterfaceModuleObject
{
	// construction
public:
	VerosimAUVInterfaceModuleState(std::string str) 
		: VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::State)
	{
		initializeFromString(str);
	};
	VerosimAUVInterfaceModuleState(double _x  , double _y  , double _z  , double _phi  , double _theta  , double _psi,
											 double _v_x, double _v_y, double _v_z, double _v_phi, double _v_theta, double _v_psi,
											 double _a_x, double _a_y, double _a_z, double _a_phi, double _a_theta, double _a_psi,
											 double _timeStamp)
		: VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::State)
	{
		x = _x; y = _y; z = _z; phi = _phi; theta = _theta; psi = _psi; 
		v_x = _v_x; v_y = _v_y; v_z = _v_z; v_phi = _v_phi; v_theta = _v_theta; v_psi = _v_psi;
		a_x = _a_x; a_y = _a_y; a_z = _a_z; a_phi = _a_phi; a_theta = _a_theta; a_psi = _a_psi;
		timeStamp = _timeStamp;
	};
	~VerosimAUVInterfaceModuleState(){};

	// services
public:
	std::string toString()
	{
		std::ostringstream os;
		os << "$State," << x << "," << y << "," << z << "," << phi << "," << theta << "," << psi << 
			"," << v_x << "," << v_y << "," << v_z << "," << v_phi << "," << v_theta << "," << v_psi << 
			"," << a_x << "," << a_y << "," << a_z << "," << a_phi << "," << a_theta << "," << a_psi << 
			"," << timeStamp << "\n";
		return os.str();
	};

	// management
private:
	void initializeFromString(std::string str) // expects: "x,y,z,phi,...,a_theta,a_psi\n"
	{
		uint i = 0;
		for(uint i = 0; i <= 17; i++) // 17 expected commas
		{
			uint index = str.find_first_of(",",0);
			std::string substring = str.substr(0, index);
			switch(i)
			{
			case 0: {x = atof(substring.c_str()); break;}
			case 1: {y = atof(substring.c_str()); break;}
			case 2: {z = atof(substring.c_str()); break;}
			case 3: {phi = atof(substring.c_str()); break;}
			case 4: {theta = atof(substring.c_str()); break;}
			case 5: {psi = atof(substring.c_str()); break;}
			case 6: {v_x = atof(substring.c_str()); break;}
			case 7: {v_y = atof(substring.c_str()); break;}
			case 8: {v_z = atof(substring.c_str()); break;}
			case 9: {v_phi = atof(substring.c_str()); break;}
			case 10:{v_theta = atof(substring.c_str()); break;}
			case 11:{v_psi = atof(substring.c_str()); break;}
			case 12:{a_x = atof(substring.c_str()); break;}
			case 13:{a_y = atof(substring.c_str()); break;}
			case 14:{a_z = atof(substring.c_str()); break;}
			case 15:{a_phi = atof(substring.c_str()); break;}
			case 16:{a_theta = atof(substring.c_str()); break;}
			case 17:{a_psi = atof(substring.c_str()); break;}
			}
			str.erase(0, index+1);
		}
		// remaining "a_psi\n"
		uint index = str.find_first_of("\n",0);
		std::string substring = str.substr(0, index);
		timeStamp = atof(substring.c_str());	
	};

	// data
public:
	double x, y, z, phi, theta, psi;			        // position and orientation in world coordinates
	double v_x, v_y, v_z, v_phi, v_theta, v_psi;	  // linear and angular velocities in body coordinates
	double a_x, a_y, a_z, a_phi, a_theta, a_psi;   // accelerations in body coordinates
	double timeStamp;										  // secs since 1970.	
}; // class VerosimAUVInterfaceModuleState

// path segment characterization
class VerosimAUVInterfaceModulePathSegmentCharacterization : public VerosimAUVInterfaceModuleObject
{
	// construction
public:
	VerosimAUVInterfaceModulePathSegmentCharacterization(double _v, double _x, double _y, double _z)
		: VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::PathSegment)
	{
		v = _v; x = _x; y = _y; z = _z;
	};
	VerosimAUVInterfaceModulePathSegmentCharacterization(std::string str)
		: VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::PathSegment)
	{
		initializeFromString(str);
	};
	~VerosimAUVInterfaceModulePathSegmentCharacterization(){};

	// services
public:
	std::string toString()
	{
		std::ostringstream os;
		os << "$PathSegment," << v << "," << x << "," << y << "," << z << "\n";
		return os.str();
	};
	void initializeFromString(std::string str) // expects "v,x,y,z\n"
	{
		uint index				 = str.find_first_of(",",0);
		std::string substring = str.substr(0, index);
		v = atof(substring.c_str());
		str.erase(0, index+1);
		index						 = str.find_first_of(",",0);
		substring			    = str.substr(0, index);
		x = atof(substring.c_str());
		str.erase(0, index+1);
		index						 = str.find_first_of(",",0);
		substring			    = str.substr(0, index);
		y = atof(substring.c_str());
		str.erase(0, index+1);
		index						 = str.find_first_of("\n",0);
		substring			    = str.substr(0, index);
		z = atof(substring.c_str());
	};
	// data
public:
	double v, x, y, z;
}; //class VerosimAUVInterfaceModulePathSegmentCharacterization

// path segment characterization
class VerosimAUVInterfaceModuleAutopilotCommand : public VerosimAUVInterfaceModuleObject
{
	// construction
public:
	VerosimAUVInterfaceModuleAutopilotCommand(double _v, double _psi, double _depth)
		: VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::AutopilotCommand)
	{
		v = _v; psi = _psi; depth = _depth;
	};
	VerosimAUVInterfaceModuleAutopilotCommand(std::string str)
		: VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::AutopilotCommand)
	{
		initializeFromString(str);
	};
	~VerosimAUVInterfaceModuleAutopilotCommand(){};

	// services
public:
	std::string toString()
	{
		std::ostringstream os;
		os << "$AutopilotCommand," << v << "," << psi << "," << depth << "\n";
		return os.str();
	};
	void initializeFromString(std::string str) // expects "v,psi,depth\n"
	{
		uint index				 = str.find_first_of(",",0);
		std::string substring = str.substr(0, index);
		v = atof(substring.c_str());
		str.erase(0, index+1);
		index						 = str.find_first_of(",",0);
		substring			    = str.substr(0, index);
		psi = atof(substring.c_str());
		str.erase(0, index+1);
		index						 = str.find_first_of("\n",0);
		substring			    = str.substr(0, index);
		depth = atof(substring.c_str());
	};
	// data
public:
	double v, psi, depth;
}; //class VerosimAUVInterfaceModuleAutopilotCommand

class VerosimAUVInterfaceModuleHandler
{
	// construction 
public:
	VerosimAUVInterfaceModuleHandler(){};
	~VerosimAUVInterfaceModuleHandler(){};

	// services
public:
	VerosimAUVInterfaceModuleObject* fromString(std::string str)
	{
		if(str.size() > 0)
		{
			if(str[0] == '$')
			{
				uint index = str.find_first_of(",",0);
				std::string substring = str.substr(1, index-1);
				str.erase(0, index+1);
				if(substring.compare("State") == 0)
				{
					return new VerosimAUVInterfaceModuleState(str);
				}
				if(substring.compare("PathSegment") == 0)
				{
					return new VerosimAUVInterfaceModulePathSegmentCharacterization(str);
				}
				if(substring.compare("AutopilotCommand") == 0)
				{
					return new VerosimAUVInterfaceModuleAutopilotCommand(str);
				}
			}
		}
		return new VerosimAUVInterfaceModuleObject(VerosimAUVInterfaceModuleObject::Invalid);
	}
}; // end VerosimAUVInterfaceModuleHandler

}; // end VerosimAUVInterfaceModule
#endif

