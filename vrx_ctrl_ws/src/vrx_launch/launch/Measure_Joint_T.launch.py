from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch.event_handlers import OnProcessStart

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    Input = Node(
            package='vrx_experiment',
            namespace='measure_joint_T',
            executable='measureT',
            name='SpecifyT_Input',
            parameters=[
            {'N': float(100)},
            {'Ang': float(0)},
            ]
        )
        
    Saver = Node(
        package='vrx_data_py',
        namespace='measure_joint_T',
        executable='JointState_Saver',
        name='JointState_Saver',
        parameters=[
            {'Length_of_Data': 10},
            ]
    )
        
    Vrx = IncludeLaunchDescription(
         PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('vrx_gz'),
                'launch', 'competition.launch.py'])
        ]),
    )
    
    Event_Handler = RegisterEventHandler(
        OnProcessStart(
            target_action=Input,
            on_start=[Vrx]
        )
    )
    
    return LaunchDescription([
        Input,
        Saver,
        Event_Handler
    ])
       
