package ketama;

import java.util.List;
import java.util.SortedMap;
import java.util.TreeMap;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public final class KetamaHash {
	public long CalcMd5SubHash(byte[] md5, int index) {
		long rv =   ((long) (md5[3 + index * 4] & 0xFF) << 24)
				  | ((long) (md5[2 + index * 4] & 0xFF) << 16)
				  | ((long) (md5[1 + index * 4] & 0xFF) << 8)
				  | ((long) (md5[0 + index * 4] & 0xFF) << 0);
		
		return rv & 0xffffffffL; /* Truncate to 32-bits */
	}

	/**
	 * Get the md5 of the given key.
	 */
	public byte[] CalcMd5(String key) {
		MessageDigest md5;
		try {
			md5 = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			throw new RuntimeException("MD5 not supported", e);
		}
		md5.reset();
		byte[] keyBytes = null;
		try {
			keyBytes = key.getBytes("UTF-8");
		} catch (UnsupportedEncodingException e) {
			throw new RuntimeException("Unknown string :" + key, e);
		}
		
		md5.update(keyBytes);
		return md5.digest();
	}


	private TreeMap<Long, String> KetamaNodeMap;
	private int RepNum = 160;
	
	// nodes: 原始物理节点
	// CopyNum: 每个节点对应的虚拟节点数
  public KetamaHash(List<String> nodes, int CopyNum) {
		KetamaNodeMap = new TreeMap<Long, String>();
		
    RepNum = CopyNum;
        
		for (String node : nodes) {
			// md5值有16Bytes，可以构造4个Integer，所以小循环四次
			for (int i = 0; i < RepNum / 4; i++) { // 大循环次数 * 小循环次数 = RepNum
				byte[] digest = CalcMd5(node + i); // 本次循环修改节点名称作为新虚拟节点的key
				for(int h = 0; h < 4; h++) {
					long m = CalcMd5SubHash(digest, h);
					KetamaNodeMap.put(m, node);
				}
			}
		}
  }

	public String GetNode(final String key) {
		byte[] sum = CalcMd5(key);
		String node = GetNodeForKey(CalcMd5SubHash(sum, 0));
		return node;
	}

	String GetNodeForKey(long hash) {
		final String node;
		Long key = hash;
		if (!KetamaNodeMap.containsKey(key)) {
			SortedMap<Long, String> tailMap = KetamaNodeMap.tailMap(key);
			if(tailMap.isEmpty()) {
				key = KetamaNodeMap.firstKey();
			} else {
				key = tailMap.firstKey();
			}
		}
		
		node=KetamaNodeMap.get(key);
		return node;
	}
}
