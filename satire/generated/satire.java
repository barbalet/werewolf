public class satire_new {
	
	// This is a contant
	
	public static final int STARTING_X = 5; 
	public static final int STARTING_Y = 5; 
	
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
		int loop = 0;
		while (loop < radius) {
			int radiusValue = 0;
			final int incrementNumber = 1; 
			radiusValue = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
			System.out.println("Some command");  
			System.out.println(radiusValue);    
			loop += incrementNumber + radiusValue;
		}
	}
	
	public static void main(String[] args) { 
		partialFind(STARTING_X, STARTING_Y, 5);
	}
	
}
