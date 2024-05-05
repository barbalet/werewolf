public class satireReal {
	
	// This is a contant
	
	public static final int STARTING_X = 5; 
	public static final int STARTING_Y = 5; 
	public static final int INCREMENTER = 1; 
	
	public static final float TWO_PI = 6; 
	
	public static int[] radiusValue = new int[5];
	
	// This is a comment
	
	static int findRadius(int centerX, int centerY, int radius, int pointX, int pointY) { 
		int multiX = centerX - pointX;
		int multiY = centerY - pointY;
		int multiXY = 0;
		multiXY = (multiX * multiX) + (multiY * multiY);
		if (multiXY >= (radius * radius)) {
			return 0;
		}
		return 1;
	}
	
	static float areaOperator(int findX, int findY, int loop) { 
		float valueOut = 0;
		valueOut = (TWO_PI * loop) / ((findX * findX) + (findY * findY));
		return valueOut;
	}
	
	static void partialFind(int centerX, int centerY, int radius) { 
		int loop = 0;
		int[] localRadius = new int[5];
		float[] localFloat = new float[5];
		while (loop < radius) {
			int findX = centerX - loop;
			int findY = centerY - loop;
			radiusValue[loop] = findRadius(centerX, centerY, radius, findX, findY);
			localFloat[loop] = areaOperator(findX, findY, loop);
			localRadius[loop] = radiusValue[loop] + INCREMENTER;
			System.out.println("Some command");  
			System.out.println(radiusValue[loop]);    
			System.out.println(localFloat[loop]);    
			loop += localRadius[loop];
		}
	}
	
	public static void main(String[] args) { 
		partialFind(STARTING_X, STARTING_Y, 5);
	}
	
}
