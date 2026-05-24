from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    return LaunchDescription([
        
        Node(
            package='vrx_experiment',
            namespace='measure_thruster_K',
            executable='slow_speed_up',
            name='MeasureK_Input',
        ),
        
        Node(
            package='vrx_data_py',
            namespace='measure_thruster_K',
            executable='JointState_Saver',
            name='JointState_Saver'
        ),
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([
                PathJoinSubstitution([
                    FindPackageShare('vrx_gz'),
                    'launch', 'competition.launch.py'])
            ]),
        )
        
    ])
       
