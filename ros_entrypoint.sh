#!/bin/bash
set -e

# Source ROS 2 setup
source /opt/ros/humble/setup.bash

# Source any workspace if present
if [ -f "/root/ros2_ws/install/setup.bash" ]; then
  source "/root/ros2_ws/install/setup.bash"
fi

exec "$@"

