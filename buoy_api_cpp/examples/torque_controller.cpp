// Copyright 2022 Open Source Robotics Foundation, Inc. and Monterey Bay Aquarium Research Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <buoy_api/examples/torque_control_policy.hpp>
#include <buoy_api/examples/torque_controller.hpp>

#include <string>
#include <memory>


PBTorqueController::PBTorqueController(const std::string & node_name)
: buoy_api::Interface<PBTorqueController>(node_name)
{
  policy_.reset(new PBTorqueControlPolicy());
  set_params();

  set_pc_pack_rate_param();
}

void PBTorqueController::power_callback(const buoy_interfaces::msg::PCRecord & data)
{
  auto request = std::make_shared<buoy_interfaces::srv::PCWindCurrCommand::Request>();
  request->wind_curr = policy_->WindingCurrentTarget(data.rpm, data.scale, data.retract);

  RCLCPP_INFO_STREAM(
    rclcpp::get_logger(
      this->get_name()),
    "WindingCurrent: f(" << data.rpm << ", " << data.scale << ", " << data.retract << ") = " <<
      request->wind_curr);

  PCWindCurrServiceCallback pc_wind_curr_callback =
    default_service_response_callback<PCWindCurrServiceCallback,
      PCWindCurrServiceResponseFuture>();

  // NOTE: Move semantics destroys local
  // pc_wind_curr_callback object
  auto response = pc_wind_curr_client_->async_send_request(request, pc_wind_curr_callback);
}


int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<PBTorqueController>("pb_torque_controller"));
  rclcpp::shutdown();

  return 0;
}
