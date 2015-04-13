package ketama;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

public class test1 {
	static Random ran = new Random();
	
	/** key's count */
	private static final Integer TEST_TIMES = 1000000;
	private static final Integer NODE_COUNT = 7;
	private static final Integer VIRTUAL_NODE_COUNT = 200;
	
	public static void main(String[] args) {
		test1 test = new test1();
		
		/** Records the times of locating node*/
		Map<String, Integer> records = new HashMap<String, Integer>();
		
		List<String> nodes = test.getStrings(NODE_COUNT);
		KetamaHash ketama = new KetamaHash(nodes, VIRTUAL_NODE_COUNT);
		
		List<String> reqKeys = test.getReqStrings();
		for (String key : reqKeys) {
			// 计算每个请求key的物理node
			String node = ketama.GetNode(key);
			
			// 记录每个节点被使用次数
			Integer times = records.get(node);
			if (times == null) {
				records.put(node, 1);
			} else {
				records.put(node, times + 1); // 累计
			}
		}
		
		System.out.println("-------------------- test1 start --------------------");
		System.out.println("Strings count : " + NODE_COUNT + ", Keys count : " + TEST_TIMES + ", Normal percent : " + (float) 100 / NODE_COUNT + "%");
		System.out.println("-------------------- boundary  ----------------------");
		for (Map.Entry<String, Integer> entry : records.entrySet()) {
			System.out.println("String name :" + entry.getKey() + " - Times : " + entry.getValue() + " - Percent : " + (float)entry.getValue() / TEST_TIMES * 100 + "%");
		}
		System.out.println("-------------------- test1 end ----------------------");
	}
	
	/**
	 * Gets the mock node by the material parameter 获取count个随机字符串集合
	 * 
	 * @param count 
	 * 		the count of node wanted
	 * @return
	 * 		the node list
	 */
	private List<String> getStrings(int count) {
		List<String> nodes = new ArrayList<String>();
		
		for (int k = 1; k <= count; k++) {
			String node = new String("node" + k);
			nodes.add(node);
		}
		
		return nodes;
	}
	
	/**
	 *	All the keys 获取TEST_TIMES个随机字符串集合	
	 */
	private List<String> getReqStrings() {
		List<String> nodes = new ArrayList<String>(TEST_TIMES);
		
		for (int i = 0; i < TEST_TIMES; i++) {
			nodes.add(generateRandomString(ran.nextInt(50)));
		}
		
		return nodes;
	}
	
	/**
	 * To generate the random string by the random algorithm 生成一个随机字符串
	 * <br>
	 * The char between 32 and 127 is normal char
	 * 
	 * @param length
	 * @return
	 */
	private String generateRandomString(int length) {
		StringBuffer sb = new StringBuffer(length);
		
		for (int i = 0; i < length; i++) {
			sb.append((char) (ran.nextInt(95) + 32));
		}
		
		return sb.toString();
	}
}
