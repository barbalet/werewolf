public class satireArray_new {
	
	// This is a contant
	
	public static final int STARTING_X = 5; 
	public static final int STARTING_Y = 5; 
	
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
	
	static void partialFind(int centerX, int centerY, int radius) { 
		final int INCREMENTER = 1; 
		int loop = 0;
		int[] localRadius = new int[5];
		while (loop < radius) {
			radiusValue[loop] = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
			localRadius[loop] = radiusValue[loop] + INCREMENTER;
			System.out.println("Some command");  
			System.out.println(radiusValue[loop]);    
			loop += localRadius[loop];
		}
	}
	
	public static void main(String[] args) { 
		partialFind(STARTING_X, STARTING_Y, 5);
	}
	
}
