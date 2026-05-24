from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    
    ns_launch_arg = DeclareLaunchArgument(
        'control_ns', 
        default_value='Control')
    ns = LaunchConfiguration('control_ns')
    
    data_ns_launch_arg = DeclareLaunchArgument(
        'data_ns', 
        default_value='Data')
    data_ns = LaunchConfiguration('data_ns')
        
    Go_Node = Node(
        package = 'vrx_controller',
        namespace = ns,
        executable = 'LOS',
        name = 'LOS',
        remappings = [
            ('/state', '/Data/state'),
            ('/error', '/Control/error'),
            ('/LOS_Angle', '/Control/LOS_Angle'),
        ]
    )
    
    Gain_Node = Node(
        package = 'vrx_controller',
        namespace = ns,
        executable = 'Gain',
        name = 'Gain',
        remappings = [
            ('/state', '/Data/state'),
            ('/LOS_Angle', '/Control/LOS_Angle'),
            ('/Chessis_Input', '/Control/Chessis_Input'),
        ]
    )
    
    PID_Node = Node(
        package = 'vrx_controller',
        namespace = ns,
        executable = 'ch_PID',
        name = 'PID',
        remappings = [
            ('/Chessis_Input', '/Control/Chessis_Input'),
        ]
    )
    
    Data_Node = Node(
        package = 'vrx_data_py',
        namespace = data_ns,
        executable = 'Data_Saver',
        name = 'Data_Saver',
        remappings = [
            ('/state', '/Data/state'),
            ('/error', '/Control/error'),
        ],
        parameters=[
            {'Length_of_Data': 3000},
            {'End_LOS': True},
        ]
    )   
    
    Fuse_Node1 = Node(
        package = 'vrx_data_cpp',
        namespace = data_ns,
        executable = 'gps_imu_fuse',
        name = 'Gps_Imu_Fuse',
        remappings = [
            ('/state', '/Data/state'),
        ]
    )
    
    Fuse_Node2 = Node(
        package = 'vrx_data_cpp',
        namespace = data_ns,
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
    	data_ns_launch_arg,
    	Go_Node,
    	Gain_Node, 
    	PID_Node,
    	Data_Node,
    	Fuse_Node1,
    	Vrx_Launch,  	        
    ])
