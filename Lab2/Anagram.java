import java.io.IOException;
import java.util.Arrays;
import java.util.StringTokenizer;
import java.util.Iterator;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class Anagram { 

    public static class AnagramMap
    //map
    extends Mapper<Object, Text, Text, Text> {

        private Text sortedKey = new Text();
        private Text word = new Text();
        public void map(Object key, Text value, Context context)

        throws IOException, InterruptedException {
            StringTokenizer itr = new StringTokenizer(value.toString());

            while (itr.hasMoreTokens()) {
                String token = itr.nextToken().toLowerCase().replaceAll("[^a-z]", "");
                char[] chars = token.toCharArray();
                Arrays.sort(chars);
                String sorted = new String(chars);

                sortedKey.set(sorted);
                word.set(token);
                context.write(sortedKey, word);
            }
        }
    }

    public static class AnagramReduce
    //reduce
    extends Reducer<Text, Text, Text, NullWritable> {
        
        private Text result = new Text();
        public void reduce(Text key, Iterable<Text> values, Context context)
        
        throws IOException, InterruptedException {
            StringBuilder sb = new StringBuilder();
            Iterator<Text> it = values.iterator();

            while (it.hasNext()) {
                sb.append(it.next().toString()).append(" ");
            }

            String group = sb.toString().trim();

            if (group.contains(" ")) {
                result.set(group);
                context.write(result, NullWritable.get());
            }
        }
    }

    

    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        Job job = Job.getInstance(conf, "Anagram Identifier");

        job.setJarByClass(Anagram.class);
        job.setMapperClass(AnagramMap.class);
        job.setReducerClass(AnagramReduce.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(Text.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}

