

use Data::Dumper;

$frames=50;
$fps=25;
$fragments=5*60*25/$frames;
$count=0;$prev=0;
@packets=();
print STDERR "Loading data\n";
while(<STDIN>){
$time=$pos="";
if(/time="(\d[^\"]+)"/){$time=$1;}
if(/pos="(\d+)"/){$pos=$1;}
if($time && $pos){
push(@packets,[$count++,$pos,$time]);
}
}

if(@packets==0){die "Can't load stats file! Feed it as STDIN";}

print STDERR "Count size\n";
for($q=1;$q<@packets;$q++){
$packets[$q-1][3]=$packets[$q][1]-$packets[$q-1][1];
}

print STDERR "Select frames\n";
@fragments=();
for($w=0;$w<@packets;$w++){
$e=$w+$frames;$bitrate=0;
for($q=$w;$w<$e;$w++){
$bitrate+=$packets[$q][3];
}
push(@fragments,[$q,$bitrate]);
}

print STDERR "Sorting\n";

@fragments=sort{$b->[1] <=> $a->[1]}@fragments;

print STDERR "Remove shit\n";

splice(@fragments,$fragments);
@fragments=sort{$a->[0] <=> $b->[0]}@fragments;

print STDERR "Add end time\n";
$fragment_size=$frames/$fps;
for($q=0;$q<@fragments;$q++){
$fragments[$q][2]=$packets[$fragments[$q]->[0]]->[2];
$fragments[$q][3]=$fragments[$q][2]+$fragment_size;
}

print STDERR "Fill gaps\n";
for($q=1;$q<@fragments;$q++){
while($q<@fragments && $fragments[$q]->[2]-$fragments[$q-1]->[3]<5){
$fragments[$q-1]->[3]=$fragments[$q]->[2]+$fragment_size;
splice(@fragments,$q,1);
}
}

print STDERR "Dump\n";
print Dumper(\@fragments);
print map{"$_->[2] - $_->[3]\n"}@fragments;
$count=0;
@files=();
foreach(@fragments){

$offset=$_->[2];
$len=int($_->[3]-$_->[2]+.5);
$filename="frag-$count.avi";
print STDERR "Saving fragment $offset ($len) to $count\n";
if(!-e($filename)){
`ffmpeg -ss $offset -i "$ARGV[0]" -t $len -c copy -an -y $filename`;
}
push(@files,$filename);
$count++;
}

splice(@files,0,10);

for($q=0;$q<@files;$q++){
$rand=int(rand()*@files);
$tmp=$files[$rand];
$files[$rand]=$files[$q];
$files[$q]=$tmp;
}

$list=join("|",@files);

`ffmpeg -i "concat:$list" -c copy -y trailer.avi`;











