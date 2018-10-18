import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.sql.Timestamp;
import java.util.Random;

public class TargetNumberRandomGenerator {

	public static void main(String[] args) throws FileNotFoundException {
		
		Random ran = new Random();
		Timestamp timestamp = new Timestamp(System.currentTimeMillis());// generate time stamp
		PrintStream out = new PrintStream(new FileOutputStream(timestamp + "TargetFile.txt"));
		PrintStream trialNumber = new PrintStream(new FileOutputStream(timestamp + "NumberofTrialInDifferentGapsize.txt"));
		
		int[][] sizeList = new int[31][3];//
		for(int i = 0; i < sizeList.length; i++) {
			for(int j = 0; j < sizeList[i].length;j++) {
				sizeList[i][j] = 0;
			}
		}
		
		int trial = 1;

		int valid = 0, invalid = 0, neutral = 0;
		
		do {
			out.println(trial);
			out.println(ran.nextInt(2));// leftRotate
			out.println(ran.nextInt(2));// rightRotate
			out.println(ran.nextInt(2));// cue direction
			
			int gapSize = ran.nextInt(31) + 5;
			out.println(gapSize);// Percentage of gap size

			if (ran.nextInt(1000) < 667) {
				if (ran.nextInt(1000) < 700) {
					out.println(1);
					valid++;
					sizeList[gapSize-5][0]++;
				} else {
					out.println(0);
					invalid++;
					sizeList[gapSize-5][1]++;
				}
			} else {
				out.println(2);
				neutral++;
				sizeList[gapSize-5][2]++;
			}

			out.println("*");
			trial++;
		} while (trial < 3600);
		
		for(int i = 0; i < sizeList.length; i++) {
			trialNumber.println(i+5);
			trialNumber.println();
			for(int j = 0; j < sizeList[i].length;j++) {
				trialNumber.println(sizeList[i][j]);
			}
			trialNumber.println("***");
		}
		out.close();
		trialNumber.close();
		System.out.println(valid + "valid, " + invalid + "invalid, " + neutral + "neutral");
	}

}
