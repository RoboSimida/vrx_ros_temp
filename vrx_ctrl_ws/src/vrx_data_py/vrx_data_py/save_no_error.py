import rclpy
from rclpy.node import Node
import numpy as np

from sensor_msgs.msg import JointState
from vrx_other_interface.msg import State, Losmsg
from std_msgs.msg import Float64


class All_Saver(Node):

	def __init__(self):
		super().__init__('All_Saver')
		self.joint_sub = self.create_subscription(
			JointState, '/wamv/joint_states', self.joint_callback, 10)
		self.state_sub = self.create_subscription(
			State, '/state', self.state_callback, 10)
			
		self.joint_sub   # prevent unused variable warning
		self.state_sub
		
		self.count = 0
		
		self.declare_parameter('Length_of_Data', 1000)
		
		self.len_data = self.get_parameter('Length_of_Data').value
		
		self.data_state = []
		self.data_joint = []
		
		
	def state_callback(self, msg):
		self.count += 1
		self.data_state.append([msg.time, 
					msg.pos.x, msg.pos.y, msg.pos.psi,
					msg.vel.u, msg.vel.v, msg.vel.psi,
					msg.acc.u, msg.acc.v, msg.acc.psi])
		if self.count < self.len_data:
			self.get_logger().info('Count "%d"' % self.count)
		elif self.count == self.len_data:
			np.savetxt('/home/cizar/Desktop/NeedRename_state.txt', np.array(self.data_state))
			np.savetxt('/home/cizar/Desktop/NeedRename_joint.txt', np.array(self.data_joint))
			self.get_logger().info('Save Txt')
			
			
				
	def joint_callback(self, msg):
		self.data_joint.append([msg.position[4], msg.velocity[5]])

			
def main(args=None):
	rclpy.init(args=args)

	minimal_subscriber = All_Saver()

	rclpy.spin(minimal_subscriber)
	minimal_subscriber.destroy_node()
	rclpy.shutdown()


if __name__ == '__main__':
	main()
