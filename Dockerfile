# Use ROS2 Humble as the base image
FROM ros:humble

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Update and install required dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3-colcon-common-extensions \
    ros-humble-cv-bridge \
    ros-humble-image-transport \
    ros-humble-camera-info-manager \
    ros-humble-v4l2-camera \
    && rm -rf /var/lib/apt/lists/*

# Create a workspace
RUN mkdir -p /ros2_ws/src
WORKDIR /ros2_ws/src

# Copy the camera node package into the workspace
COPY ./dual_camera_node /ros2_ws/src/dual_camera_node

# Build the workspace
WORKDIR /ros2_ws
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && \
    colcon build --symlink-install"

# Set up the entrypoint
COPY ./ros_entrypoint.sh /

# Set up the environment
RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc && \
    echo "source /ros2_ws/install/setup.bash" >> ~/.bashrc

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]