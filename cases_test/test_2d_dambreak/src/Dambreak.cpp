/**
 * @file 	Dambreak.cpp
 * @brief 	2D dambreak exaple.
 * @details This is the one of the basic test cases, also the first case for
 * 			understanding SPH method for fluid similation.
 * @author 	Luhui Han, Chi Zhang and Xiangyu Hu
 * @version 0.1
 */
 /**
  * @brief 	SPHinXsys Library.
  */
#include "sphinxsys.h"
  /**
 * @brief Namespace cite here.
 */
using namespace SPH;
/**
 * @brief Basic geometry parameters and numerical setup.
 */
Real DL = 5.366; 						/**< Tank length. */
Real DH = 5.366; 						/**< Tank height. */
Real LL = 2.0; 							/**< Liquid colume length. */
Real LH = 1.0; 							/**< Liquid colume height. */
Real particle_spacing_ref = 0.025; 		/**< Initial reference particle spacing. */
Real BW = particle_spacing_ref * 4; 	/**< Extending width for BCs. */
/**
 * @brief Material properties of the fluid.
 */
Real rho0_f = 1.0;						/**< Reference density of fluid. */
Real gravity_g = 1.0;					/**< Gravity force of fluid. */
Real U_f = 2.0*sqrt(gravity_g*LH);		/**< Characteristic velocity. */
Real c_f = 10.0*U_f;					/**< Reference sound speed. */

Real initial_pressure = 0.0;			/**< Initial pressure field. */
Vec2d intial_velocity(0.0, 0.0);		/**< Initial velocity field. */
/**
 * @brief 	Fluid body definition.
 */
class WaterBlock : public FluidBody
{
public:
	WaterBlock(SPHSystem &system, string body_name,
		WeaklyCompressibleFluid &material,
		FluidParticles &fluid_particles, int refinement_level, ParticlesGeneratorOps op)
		: FluidBody(system, body_name, material, fluid_particles, refinement_level, op)
	{
		/** Geomerty definition. */
		std::vector<Point> water_block_shape;
		water_block_shape.push_back(Point(0.0, 0.0));
		water_block_shape.push_back(Point(0.0, LH));
		water_block_shape.push_back(Point(LL, LH));
		water_block_shape.push_back(Point(LL, 0.0));
		water_block_shape.push_back(Point(0.0, 0.0));
		Geometry *water_block_geometry = new Geometry(water_block_shape);
		body_region_.add_geometry(water_block_geometry, RegionBooleanOps::add);

		body_region_.done_modeling();
	}
};
/**
 * @brief 	Wall boundary body definition.
 */
class WallBoundary : public SolidBody
{
public:
	WallBoundary(SPHSystem &system, string body_name,
		SolidParticles &solid_particles, int refinement_level, ParticlesGeneratorOps op)
		: SolidBody(system, body_name, *(new Solid("EmptyWallMaterial")), solid_particles, refinement_level, op)
	{
		/** Geomerty definition. */
		std::vector<Point> outer_wall_shape;
		outer_wall_shape.push_back(Point(-BW, -BW));
		outer_wall_shape.push_back(Point(-BW, DH + BW));
		outer_wall_shape.push_back(Point(DL + BW, DH + BW));
		outer_wall_shape.push_back(Point(DL + BW, -BW));
		outer_wall_shape.push_back(Point(-BW, -BW));
		Geometry *outer_wall_geometry = new Geometry(outer_wall_shape);
		body_region_.add_geometry(outer_wall_geometry, RegionBooleanOps::add);

		std::vector<Point> inner_wall_shape;
		inner_wall_shape.push_back(Point(0.0, 0.0));
		inner_wall_shape.push_back(Point(0.0, DH));
		inner_wall_shape.push_back(Point(DL, DH));
		inner_wall_shape.push_back(Point(DL, 0.0));
		inner_wall_shape.push_back(Point(0.0, 0.0));
		Geometry *inner_wall_geometry = new Geometry(inner_wall_shape);
		body_region_.add_geometry(inner_wall_geometry, RegionBooleanOps::sub);

		body_region_.done_modeling();
	}
};
/**
 * @brief 	Fluid observer body definition.
 */
class FluidObserver : public ObserverEulerianBody
{
public:
	FluidObserver(SPHSystem &system, string body_name,
		ObserverParticles &observer_particles, int refinement_level, ParticlesGeneratorOps op)
		: ObserverEulerianBody(system, body_name, observer_particles, refinement_level, op)
	{
		body_input_points_volumes_.push_back(make_pair(Point(DL, 0.2), 0.0));
	}
};
/**
 * @brief 	Main program starts here.
 */
int main()
{
	/**
	 * @brief Build up -- a SPHSystem --
	 */
	SPHSystem system(Vec2d(-BW, -BW), Vec2d(DL + BW, DH + BW), particle_spacing_ref);
	/** Set the starting time. */
	GlobalStaticVariables::physical_time_ = 0.0;
	/** Tag for computation from restart files. 0: not from restart files. */
	system.restart_step_ = 0;
	/**
	 * @brief Material property, partilces and body creation of fluid.
	 */
	WeaklyCompressibleFluid 			fluid("Water", rho0_f, c_f);
	FluidParticles 	fluid_particles("WaterBody");
	WaterBlock *water_block = new WaterBlock(system, "WaterBody", fluid,
		fluid_particles, 0, ParticlesGeneratorOps::lattice);
	/**
	 * @brief 	Particle and body creation of wall boundary.
	 */
	SolidParticles 					solid_particles("Wall");
	WallBoundary *wall_boundary = new WallBoundary(system, "Wall",
		solid_particles, 0, ParticlesGeneratorOps::lattice);
	/**
	 * @brief 	Particle and body creation of fluid observer.
	 */
	ObserverParticles 					observer_particles("Fluidobserver");
	FluidObserver *fluid_observer = new FluidObserver(system, "Fluidobserver",
		observer_particles, 0, ParticlesGeneratorOps::direct);
	/**
	 * @brief 	Body contact map.
	 * @details The contact map gives the data conntections between the bodies.
	 * 			Basically the the rang of bidies to build neighbor particle lists.
	 */
	SPHBodyTopology 	body_topology = { { water_block, { wall_boundary } },
										  { wall_boundary, {} },{ fluid_observer,{ water_block} } };
	system.SetBodyTopology(&body_topology);
	/**
	 * @brief 	Simulation set up.
	 */
	system.SetupSPHSimulation();
	/**
	 * @brief 	Define all numerical methods which are used in this case.
	 */
	 /** Define external force. */
	Gravity 							gravity(Vecd(0.0, -gravity_g));
	 /**
	  * @brief 	Methods used only once.
	  */
	  /** initial condition for the solid body */
	solid_dynamics::SolidDynamicsInitialCondition set_all_solid_particles_at_rest(wall_boundary);
	/** Initialize normal direction of the wall boundary. */
	solid_dynamics::NormalDirectionSummation 	get_wall_normal(wall_boundary, {});

	/** initial condition for fluid body */
	fluid_dynamics::WeaklyCompressibleFluidInitialCondition set_all_fluid_particles_at_rest(water_block);
	/** Obtain the initial number density of fluid. */
	fluid_dynamics::InitialNumberDensity 		fluid_initial_number_density(water_block, { wall_boundary });
	/**
	 * @brief 	Methods used for time stepping.
	 */
	 /** Initialize particle acceleration. */
	InitializeOtherAccelerations 	initialize_fluid_acceleration(water_block, &gravity);
	/**
	 * @brief 	Algorithms of fluid dynamics.
	 */
	 /** Wvaluation of density by summation approach. */
	fluid_dynamics::DensityBySummationFreeSurface 		update_fluid_desnity(water_block, { wall_boundary });
	/** Time step size without considering sound wave speed. */
	fluid_dynamics::GetAdvectionTimeStepSize 			get_fluid_adevction_time_step_size(water_block, U_f);
	/** Time step size with considering sound wave speed. */
	fluid_dynamics::GetAcousticTimeStepSize get_fluid_time_step_size(water_block);
	/** Pressure relaxation algorithm by using verlet time stepping. */
	fluid_dynamics::PressureRelaxationVerletFreeSurface pressure_relaxation(water_block, { wall_boundary }, &gravity);
	/**
	 * @brief 	Methods used for updating data structure.
	 */
	 /** Update the cell linked list of bodies when neccessary. */
	ParticleDynamicsCellLinkedList			update_cell_linked_list(water_block);
	/** Update the configuration of bodies when neccessary. */
	ParticleDynamicsConfiguration 			update_particle_configuration(water_block);
	/** Update the interact configuration of bodies when neccessary. */
	ParticleDynamicsInteractionConfiguration 	
		update_observer_interact_configuration(fluid_observer, { water_block });
	/**
	 * @brief Output.
	 */
	In_Output in_output(system);
	/** Output the body states. */
	WriteBodyStatesToVtu 		write_body_states(in_output, system.real_bodies_);
	/** Output the body states for restart simulation. */
	ReadRestart		read_restart_files(in_output, system.real_bodies_);
	WriteRestart	write_restart_files(in_output, system.real_bodies_);
	/** Output the mechanical energy of fluid body. */
	WriteWaterMechanicalEnergy 	write_water_mechanical_energy(in_output, water_block, &gravity);
	/** output the observed data from fluid body. */
	WriteObservedFluidPressure	write_recorded_water_pressure(in_output, fluid_observer, { water_block });
	
	/**
	 * @brief Setup goematrics and initial conditions
	 */
	set_all_fluid_particles_at_rest.exec();
	set_all_solid_particles_at_rest.exec();
	get_wall_normal.exec();
	fluid_initial_number_density.exec();

	/**
	 * @brief The time stepping starts here.
	 */
	 /** If the starting time is not zero, please setup the restart time step ro read in restart states. */
	if (system.restart_step_ != 0)
	{
		GlobalStaticVariables::physical_time_ = read_restart_files.ReadRestartFiles(system.restart_step_);
		update_cell_linked_list.parallel_exec();
		update_particle_configuration.parallel_exec();
	}
	/** Output the start states of bodies. */
	write_body_states.WriteToFile(GlobalStaticVariables::physical_time_);
	/** Output the Hydrostatic mechanical energy of fluid. */
	write_water_mechanical_energy.WriteToFile(GlobalStaticVariables::physical_time_);
	/**
	 * @brief 	Basic parameters.
	 */
	int number_of_iterations = system.restart_step_;
	int screen_output_interval = 100;
	int restart_output_interval = screen_output_interval*10;
	Real End_Time = 20.0; 	/**< End time. */
	Real D_Time = 0.1;		/**< Time stamps for output of body states. */
	Real Dt = 0.0;			/**< Default advection time step sizes. */
	Real dt = 0.0; 			/**< Default accoustic time step sizes. */
	/** statistics for computing CPU time. */
	tick_count t1 = tick_count::now();
	tick_count::interval_t interval;
	/**
	 * @brief 	Main loop starts here.
	 */
	while (GlobalStaticVariables::physical_time_ < End_Time)
	{
		Real integeral_time = 0.0;
		/** Integrate time (loop) until the next output time. */
		while (integeral_time < D_Time)
		{
			/** Acceleration due to viscous force and gravity. */
			initialize_fluid_acceleration.parallel_exec();
			Dt = get_fluid_adevction_time_step_size.parallel_exec();
			update_fluid_desnity.parallel_exec();

			/** Dynamics including pressure relaxation. */
			Real relaxation_time = 0.0;
			while (relaxation_time < Dt)
			{
				pressure_relaxation.parallel_exec(dt);
				dt = get_fluid_time_step_size.parallel_exec();
				relaxation_time += dt;
				integeral_time += dt;
				GlobalStaticVariables::physical_time_ += dt;

			}

			if (number_of_iterations % screen_output_interval == 0)
			{
				cout << fixed << setprecision(9) << "N=" << number_of_iterations << "	Time = "
					<< GlobalStaticVariables::physical_time_
					<< "	Dt = " << Dt << "	dt = " << dt << "\n";

				if (number_of_iterations % restart_output_interval == 0)
					write_restart_files.WriteToFile(Real(number_of_iterations));
			}
			number_of_iterations++;

			/** Update cell linked list and configuration. */
			update_cell_linked_list.parallel_exec();
			update_particle_configuration.parallel_exec();
			update_observer_interact_configuration.parallel_exec();
		}

		tick_count t2 = tick_count::now();
		write_water_mechanical_energy.WriteToFile(GlobalStaticVariables::physical_time_);
		write_body_states.WriteToFile(GlobalStaticVariables::physical_time_);
		write_recorded_water_pressure.WriteToFile(GlobalStaticVariables::physical_time_);
		tick_count t3 = tick_count::now();
		interval += t3 - t2;
	}
	tick_count t4 = tick_count::now();

	tick_count::interval_t tt;
	tt = t4 - t1 - interval;
	cout << "Total wall time for computation: " << tt.seconds()
		<< " seconds." << endl;

	return 0;
}
