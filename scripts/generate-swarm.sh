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
  if [ "$FORMATION" == "flying_v" ]; then
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
  elif [ "$FORMATION" == "helix" ] || [ "$FORMATION" == "carousel" ]; then
    PHASE=$(LC_ALL=C awk -v i="$i" -v size="$SWARM_SIZE" 'BEGIN {print (2.0 * 3.14159 * (i - 1)) / size}')
    X_OFFSET=$(LC_ALL=C awk -v phase="$PHASE" 'BEGIN {printf "%.2f", 3.0 * cos(phase)}')
    Y_OFFSET=$(LC_ALL=C awk -v phase="$PHASE" 'BEGIN {printf "%.2f", 3.0 * sin(phase)}')  
  else
    X_OFFSET=0
    Y_OFFSET=$(( (i - 1) * 2 ))
  fi

  cat << EOF >> $OUT_FILE

  <include file="\$(find flie_swarm_core)/launch/spawn_drone.launch">
    <arg name="drone_name" value="drone_${i}"/>
    <arg name="x" value="${X_OFFSET}"/>
    <arg name="y" value="${Y_OFFSET}"/>
    <arg name="drone_id" value="${i}"/>
    <arg name="swarm_size" value="${SWARM_SIZE}"/>
    <arg name="swarm_formation" value="${FORMATION}"/>
  </include>
EOF
done

echo "</launch>" >> $OUT_FILE
echo "Generated $OUT_FILE for $SWARM_SIZE drones in $FORMATION formation."
