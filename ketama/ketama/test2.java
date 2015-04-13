package ketama;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

public class test2 {
	static Random ran = new Random();
	
	/** key's count */
	private static final Integer TEST_TIMES = 100000;
	
	private static final Integer NODE_COUNT = 50;
	
	private static final Integer VIRTUAL_NODE_COUNT = 200;
	
	static List<String> reqKeys = null;
	
	static {
		reqKeys = getReqStrings();
	}
	
	public static void main(String[] args) {
		Map<String, List<String>> map = generateRecord(); // key是TEST_TIMES个随机请求值，而list则是在下面三种情况下实际使用的node的集合
		
		System.out.println("-------------------- test2 start --------------------");
		List<String> allNodes = getNodes(NODE_COUNT);
		System.out.println("Normal case : nodes count : " + allNodes.size());
		call(allNodes, map);
		
		allNodes = getNodes(NODE_COUNT + 8);
		System.out.println("Added case : nodes count : " + allNodes.size());
		call(allNodes, map); // 计算增加key的时候，每个请求key的物理node
		
		allNodes = getNodes(NODE_COUNT - 10);
		System.out.println("Reduced case : nodes count : " + allNodes.size());
		call(allNodes, map); // 计算减少key的时候，每个请求key的物理node
		
		int addCount = 0;
		int reduceCount = 0;
		for (Map.Entry<String, List<String>> entry : map.entrySet()) {
			List<String> list = entry.getValue();
			
			if (list.size() == 3) {
				if (list.get(0).equals(list.get(1))) { // 如果node1和node0相同，则说明增加node的情况下这个key依然命中了同样的物理node
					addCount++;
				}
				
				if (list.get(0).equals(list.get(2))) { // 如果node2和node0相同，则说明减少node的情况下这个key依然命中了同样的物理node
					reduceCount++;
				}
			} else {
				// 如果list的size不为3，说明在某次计算中没有得到实际的物理node，这是不应该的
				System.out.println("It's wrong size of list, key is " + entry.getKey() + ", size is " + list.size());
			}
		}
		
		System.out.println(addCount + "   ---   " + reduceCount);
		
		System.out.println("Same percent in added case : " + (float) addCount * 100/ TEST_TIMES + "%");
		System.out.println("Same percent in reduced case : " + (float) reduceCount * 100/ TEST_TIMES + "%");
		System.out.println("-------------------- test2 end ----------------------");
	}
	
	private static void call(List<String> nodes, Map<String, List<String>> map) {
		KetamaHash locator = new KetamaHash(nodes, VIRTUAL_NODE_COUNT);
		
		for (Map.Entry<String, List<String>> entry : map.entrySet()) {
			String node = locator.GetNode(entry.getKey());
			
			if (node != null) {
				List<String> list = entry.getValue();
				list.add(node);
			}
		}
	}
	
	private static Map<String, List<String>> generateRecord() {
		Map<String, List<String>> record = new HashMap<String, List<String>>(TEST_TIMES);
		
		for (String key : reqKeys) {
			List<String> list = record.get(key);
			if (list == null) {
				list = new ArrayList<String>();
				record.put(key, list);
			}
		}
		
		return record;
	}
	
	
	/**
	 * Gets the mock node by the material parameter
	 * 
	 * @param nodeCount 
	 * 		the count of node wanted
	 * @return
	 * 		the node list
	 */
	private static List<String> getNodes(int nodeCount) {
		List<String> nodes = new ArrayList<String>();
		
		for (int k = 1; k <= nodeCount; k++) {
			String node = new String("node" + k);
			nodes.add(node);
		}
		
		return nodes;
	}
	
	/**
	 *	All the keys 获取TEST_TIMES个随机字符串集合	
	 */
	private static List<String> getReqStrings() {
		List<String> allStrings = new ArrayList<String>(TEST_TIMES);
		
		for (int i = 0; i < TEST_TIMES; i++) {
			allStrings.add(generateRandomString(ran.nextInt(50)));
		}
		
		return allStrings;
	}
	
	/**
	 * To generate the random string by the random algorithm 生成一个随机字符串
	 * <br>
	 * The char between 32 and 127 is normal char
	 * 
	 * @param length
	 * @return
	 */
	private static String generateRandomString(int length) {
		StringBuffer sb = new StringBuffer(length);
		
		for (int i = 0; i < length; i++) {
			sb.append((char) (ran.nextInt(95) + 32));
		}
		
		return sb.toString();
	}
}
