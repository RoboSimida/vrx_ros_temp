from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_context import LaunchContext
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    
    sysIden_straight_ns_launch_arg = DeclareLaunchArgument(
        'SysIden_Straight_ns', 
        default_value='System_Identify')
    sysIden_straight_ns = LaunchConfiguration('SysIden_Straight_ns')
        
    Go_Node = Node(
        package = 'vrx_experiment',
        namespace = sysIden_straight_ns,
        executable = 'Fang', 
        name = 'Input'
    )
    
    Save_Data_Node = Node(
        package = 'vrx_data_py',
        namespace = sysIden_straight_ns,
        executable = 'IRLS1_Saver',
        name = 'Data_Saver',
        remappings = [
            ('/state', 
            '/System_Identify/state'),
        ]
    )   
    
    Save_Input_Node = Node(
        package = 'vrx_data_py',
        namespace = sysIden_straight_ns,
        executable = 'Input_Saver',
        name = 'Input_Saver'
    )    
    
    Fuse_Node1 = Node(
        package = 'vrx_data_cpp',
        namespace = sysIden_straight_ns,
        executable = 'gps_imu_fuse',
        name = 'Gps_Imu_Fuse'
    )
    
    Fuse_Node2 = Node(
        package = 'vrx_data_cpp',
        namespace = sysIden_straight_ns,
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
    	sysIden_straight_ns_launch_arg,
    	Go_Node,
    	Save_Data_Node,
    	Save_Input_Node,
    	Fuse_Node2,
    	Vrx_Launch,  	        
    ])
