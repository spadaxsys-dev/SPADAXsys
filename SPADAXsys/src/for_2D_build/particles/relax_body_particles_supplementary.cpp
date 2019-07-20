#include "relax_body_particles.h"

#include <iterator>

using namespace std;

namespace SPH {
	//===========================================================//
	void RelaxBodyParticles::WriteParticlesToVtuFile(ofstream &output_file)
	{
		size_t number_of_particles = base_particle_data_.size();
		output_file << "  <Piece Name =\"" << body_name_ << "\" NumberOfPoints=\"" << number_of_particles << "\" NumberOfCells=\"0\">\n";

		//write coordinates of particles
		output_file << "   <Points>\n";
		output_file << "    <DataArray Name=\"Position\" type=\"Float32\"  NumberOfComponents=\"3\" Format=\"ascii\">\n";
		output_file << "    ";
		for (size_t i = 0; i != number_of_particles; ++i) {
			output_file << base_particle_data_[i].pos_n_[0] << " " << base_particle_data_[i].pos_n_[1] << " " << 0.0 << " ";
		}
		output_file << std::endl;
		output_file << "    </DataArray>\n";
		output_file << "   </Points>\n";

		//write data of particles
		output_file << "   <PointData  Vectors=\"vector\">\n";
		output_file << "    <DataArray Name=\"Particle_ID\" type=\"Int32\" Format=\"ascii\">\n";
		output_file << "    ";
		for (size_t i = 0; i != number_of_particles; ++i) {
			output_file << base_particle_data_[i].particle_ID_ << " ";
		}
		output_file << std::endl;
		output_file << "    </DataArray>\n";

		output_file << "   </PointData>\n";

		//write empty cells
		output_file << "   <Cells>\n";
		output_file << "    <DataArray type=\"Int32\"  Name=\"connectivity\"  Format=\"ascii\">\n";
		output_file << "    </DataArray>\n";
		output_file << "    <DataArray type=\"Int32\"  Name=\"offsets\"  Format=\"ascii\">\n";
		output_file << "    </DataArray>\n";
		output_file << "    <DataArray type=\"types\"  Name=\"offsets\"  Format=\"ascii\">\n";
		output_file << "    </DataArray>\n";
		output_file << "   </Cells>\n";

		output_file << "  </Piece>\n";
	}
	//===========================================================//
	void RelaxBodyParticles::WriteParticlesToPltFile(ofstream &output_file)
	{
		output_file << " VARIABLES = \" x \", \"y\", \"ID\" \n";

		size_t number_of_particles = base_particle_data_.size();
		for (size_t i = 0; i != number_of_particles; ++i)
		{
			output_file << base_particle_data_[i].pos_n_[0] << "  "
				<< base_particle_data_[i].pos_n_[1] << "  "
				<< base_particle_data_[i].particle_ID_ << "\n ";
		}
	}
	//===========================================================//
	void RelaxBodyParticles::WriteParticlesToXmlFile(std::string &filefullpath)
	{
		XmlEngine* relax_xml = new XmlEngine("particles_xml", "particles");
		
		size_t number_of_particles = base_particle_data_.size();
		for(size_t i = 0; i != number_of_particles; ++i)
  		{
  			relax_xml->CreatXmlElement("particle");
    		relax_xml->AddAttributeToElement("ID",base_particle_data_[i].particle_ID_);
    		relax_xml->AddAttributeToElement("Position",base_particle_data_[i].pos_n_);
    		relax_xml->AddAttributeToElement("Volume",base_particle_data_[i].Vol_);
    		relax_xml->AddElementToXmlDoc();
  		}
  		relax_xml->WriteToXmlFile(filefullpath);
	}
	//===========================================================//
}
