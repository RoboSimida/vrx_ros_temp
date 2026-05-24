from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    
    ns_launch_arg = DeclareLaunchArgument(
        'round_ns', 
        default_value='Round')
    ns = LaunchConfiguration('round_ns')
        
    Go_Node = Node(
        package = 'vrx_experiment',
        namespace = ns,
        executable = 'round',
        name = 'Input'
    )
    
    Data_Node = Node(
        package = 'vrx_data_py',
        namespace = ns,
        executable = 'Data2_Saver',
        name = 'Data_Saver',
        remappings = [
            ('/state', '/Round/state'),
        ],
        parameters=[
            {'Length_of_Data': 500},
        ]
    )   
    
    Fuse_Node1 = Node(
        package = 'vrx_data_cpp',
        namespace = ns,
        executable = 'gps_imu_fuse',
        name = 'Gps_Imu_Fuse'
    )
    
    Fuse_Node2 = Node(
        package = 'vrx_data_cpp',
        namespace = ns,
        executable = 'fuse_substisute',
        name = 'Fuse'
    )
    
      
    Vrx_Launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('vrx_gz'),
                'launch', 'competition.launch.py'])
        ]),
    )
    
    return LaunchDescription([
    	ns_launch_arg,
    	Go_Node,
    	Data_Node,
    	Fuse_Node2,
    	Vrx_Launch,  	        
    ])
