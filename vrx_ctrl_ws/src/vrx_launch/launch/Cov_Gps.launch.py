from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    return LaunchDescription([
        
        Node(
            package='vrx_data_py',
            namespace='Cov_Gps',
            executable='Gps_Saver',
            name='Gps_Saver'
        ),
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([
                PathJoinSubstitution([
                    FindPackageShare('vrx_gz'),
                    'launch', 'competition.launch.py'])
            ]),
        )
        
    ])
