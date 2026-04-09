#!/usr/bin/env bash

SWARM_SIZE=${1:-5}
FORMATION=${2:-flying_v}
OUT_FILE="../launch/swarm_generated.launch"

cat << EOF > $OUT_FILE
<?xml version="1.0"?>
<launch>
  <include file="\$(find gazebo_ros)/launch/empty_world.launch">
    <arg name="world_name" value="worlds/empty_sky.world"/>
  </include>
EOF

for i in $(seq 1 $SWARM_SIZE); do
  if [ "$FORMATION" == "v_shape" ]; then
      if [ $i -eq 1 ]; then
        X_OFFSET=0
        Y_OFFSET=0
      else
        RANK=$(( i / 2 ))
        IS_LEFT=$(( i % 2 ))
        X_OFFSET=$(( RANK * -2 ))
        if [ $IS_LEFT -eq 0 ]; then
          Y_OFFSET=$(( RANK * 2 ))
        else
          Y_OFFSET=$(( RANK * -2 ))
        fi
      fi
    else
      X_OFFSET=0
      Y_OFFSET=$(( (i - 1) * 2 ))
    fi

    cat << EOF >> $OUT_FILE

  <include file="\$(find flie_swarm_core)/launch/spawn_drone.launch">
    <arg name="drone_name" value="drone_${i}"/>
    <arg name="x" value="${X_OFFSET}.0"/>
    <arg name="y" value="${Y_OFFSET}.0"/>
    <arg name="drone_id" value="${i}"/>
    <arg name="swarm_size" value="${SWARM_SIZE}"/>
    <arg name="swarm_formation" value="${FORMATION}"/>
  </include>
EOF
done

echo "</launch>" >> $OUT_FILE
echo "Generated $OUT_FILE for $SWARM_SIZE drones."
