/**
 * @file base_particles.cpp
 * @brief Definition of funcitons declared in base_particles.h
 * @author	Xiangyu Hu and Chi Zhang
 * @version	0.1
 */
#include "base_particles.h"

namespace SPH
{
	//===============================================================//
	int BaseParticleData::ID_max_ = 0;
	//===============================================================//
	BaseParticleData::BaseParticleData(Vecd position, Real volume) 
		: vel_n_(0), pos_n_(position), Vol_(volume), Vol_0_(volume),
		cell_location_(0), dvel_dt_others_(0), dvel_dt_(0)
	{
		particle_ID_ = ID_max_;
		ID_max_++;
	}
	//===============================================================//
	Particles::Particles(string body_name) 
		: body_name_(body_name)
	{

	}
	//===============================================================//
	void Particles::WriteToXmlForReloadParticle(std::string &filefullpath)
	{
		const SimTK::String xml_name("particles_xml"), ele_name("particles");
		XmlEngine* reload_xml = new XmlEngine(xml_name, ele_name);

		size_t number_of_particles = base_particle_data_.size();
		for (size_t i = 0; i != number_of_particles; ++i)
		{
			reload_xml->CreatXmlElement("particle");
			reload_xml->AddAttributeToElement("Position", base_particle_data_[i].pos_n_);
			reload_xml->AddAttributeToElement("Volume", base_particle_data_[i].Vol_);
			reload_xml->AddElementToXmlDoc();
		}
		reload_xml->WriteToXmlFile(filefullpath);
	}
	//===============================================================//
	void Particles::ReadFromXmlForReloadParticle(std::string &filefullpath)
	{
		size_t number_of_particles = 0;
		XmlEngine* read_xml = new XmlEngine();
		read_xml->LoadXmlFile(filefullpath);
		SimTK::Xml::element_iterator ele_ite_ = read_xml->root_element_.element_begin();
		for (; ele_ite_ != read_xml->root_element_.element_end(); ++ele_ite_)
		{
			Vecd position = read_xml->GetRequiredAttributeVectorValue(ele_ite_, "Position");
			base_particle_data_[number_of_particles].pos_n_ = position;
			Real volume = read_xml->GetRequiredAttributeRealValue(ele_ite_, "Volume");
			base_particle_data_[number_of_particles].Vol_ = volume;
			number_of_particles++;
		}

		if(number_of_particles != base_particle_data_.size())
		{
			std::cout << "\n Error: reload particle number does not matrch" << std::endl;
			std::cout << __FILE__ << ':' << __LINE__ << std::endl;
			exit(1);
		}
	}
	//===============================================================//
	Particles* Particles::PointToThisObject()
	{
		return this;
	}
}