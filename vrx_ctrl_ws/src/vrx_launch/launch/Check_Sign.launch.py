from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_context import LaunchContext
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    
    check_sign_ns_launch_arg = DeclareLaunchArgument(
        'Check_Sign_ns', 
        default_value='Check_Sign')
    check_sign_ns = LaunchConfiguration('Check_Sign_ns')
        
    Go_Node1 = Node(
        package = 'vrx_experiment',
        namespace = check_sign_ns,
        executable = 'SysIden', 
        name = 'Input'
    )
    
    Go_Node2 = Node(
        package = 'vrx_controller',
        namespace = check_sign_ns,
        executable = 'Go_Ctrl', 
        name = 'Input'
    )
    
    Check_Node = Node(
        package = 'vrx_data_cpp',
        namespace = check_sign_ns,
        executable = 'check_sign',
        name = 'Checker',
        remappings = [
            ('/state', 
            '/Check_Sign/state'),
        ]
    )    
    
    Fuse_Node = Node(
        package = 'vrx_data_cpp',
        namespace = check_sign_ns,
        executable = 'gps_imu_fuse',
        name = 'Gps_Imu_Fuse'
    )
      
    Vrx_Launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('vrx_gz'),
                'launch', 'competition.launch.py'])
        ]),
    )
    
    return LaunchDescription([
    	check_sign_ns_launch_arg,
    	Go_Node2,
    	Check_Node,
    	Fuse_Node,
    	Vrx_Launch,  	        
    ])
