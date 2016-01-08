///////////////////////////////////////////////////////////////////////////////
// communication.h - Header for communication - Dynamics Realtime Obstacle Pathing System
//Copyright (C) 2015  Christopher Newport University
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cpprest/http_client.h>

#include <boost/asio.hpp>

#include <mutex>

#ifndef HOST
#define HOST "http://private-6dd53-jamapi.apiary-mock.com/"
#endif

// The threshold for obstacles in the cost map. 0-255
#define OBSTACLE_THRES 255

// The number of map units to inflate the radius of obstacles
#define DEFAULT_INFLATION_RADIUS 6
// The default for the inflation weight value (should be a double)
#define DEFAULT_WEIGHT 0.6

using namespace web::http::client;

struct env_data_t
{
  int height; //y
  int width; //x
  int start_x;
  int start_y;
  int start_theta;
  int end_x;
  int end_y;
  int end_theta;
  unsigned char** grid_2d;
};

struct env_constants_t
{
  unsigned char obsthresh;
  unsigned char cost_inscribed_thresh;
  int cost_possibly_circumscribed_thresh;
  double nominalvel_mpersecs;
  double timetoturn45degsinplace_secs;

  double cellsize_m;
  const char* motion_prim_file;
};

struct inflation_params_t
{
  int inflation_radius;
  double weight;
};

class communicator
{
public:
  // Constructor
  communicator();
  // Destructor
  virtual ~communicator();

  // Starts the background task to update the m_env_data from _JAM
  void update_data();
  // Returns true is the data has been updated since the last call to update_data()
  bool is_updated();
  // Returns true if the update is still in progress
  bool update_in_progress();
  // Returns the data
  env_data_t get_env_data();
  // The lock and unlock are for the grid_2d data (which is passed by pointer)
  // lock must be called before the grid_2d data is used, and unlock after use is finished
  // this ensure thread safe usage of that data.
  bool lock_env_grid_2d();
  bool unlock_env_grid_2d();

  void post_results(); //This function needs to be updated to get the data passed to it.
  bool is_posted();
  bool post_in_progress();

private:

  bool grid_had_changed; //True when the grid has been changed size in the most recent request
                         //also true when the grid is unset
                         //Used to create a new search grid, rather than update an existing one.
  bool m_updated;
  bool m_posted;

  env_constants_t m_env_constants;

  std::mutex m_env_data_mutex; //Mutex for locking m_env_data when editing
  env_data_t m_env_data;

  //Task objects
  pplx::task<void> m_task_update;   //Task for updating everything

  //Task Generators - Return task objects
  pplx::task<void> get_grid(); //Returns a task for getting grid info

  http_client m_client;
  http_client_config m_client_config;

  // Obstacle inflation parameters
  inflation_params_t m_inflation_params;

  //Private struct used internally for obstacles
  struct obstacle_t
  {
    int x;
    int y;
    int radius;
    int heading;
    int velocity;
  };

};


#endif /* COMMUNICATION_H */
