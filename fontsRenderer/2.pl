





use Data::Dumper;

# int32 - charcode
# int32 - x
# int32 - x2
# int32 - y
# int32 - y2
# int32 - pixels 50x50

use GD;
$pic=new GD::Image(500,256);
$white=$pic->colorAllocate(255,255,255);
$black=$pic->colorAllocate(0,0,0);
$red=$pic->colorAllocate(255,0,0);
$green=$pic->colorAllocate(0,255,0);
$yellow=$pic->colorAllocate(255,255,0);
$pic->filledRectangle(0,0,500,256,$red);

for($q=0;$q<256;$q+=16){
$pic->rectangle(0,$q,500,$q,$green);
}


$lastcode=0xFFFF;
$font_id=-1;

$pos=0;
$pos_x=0;
$pos_y=0;

$min_top=0xFFFF;

my @font=();
my @fonts=();

open(dd, "out.bin");
binmode(dd);

while(!eof(dd)){
read(dd,$num,4);
$charcode=unpack("I",$num);

read(dd,$num,4);
$min_x=unpack("I",$num);

read(dd,$num,4);
$max_x=unpack("I",$num);
read(dd,$num,4);
$min_y=unpack("I",$num);

read(dd,$num,4);
$max_y=unpack("I",$num);

read(dd,$pixels,50*50*4);

if($lastcode>$charcode){
print " $lastcode>$charcode -> new font\n";
save_font();
$font_id++;

}
$lastcode=$charcode;

#if($charcode<65 || $charcode>75){next;}

($width,$height)=($max_x-$min_x+1,$max_y-$min_y+1);


$pix="";
for($w=0;$w<$height;$w++){
for($q=0;$q<$width;$q++){
$pix.=substr($pixels,($q+$min_x+($w+$min_y)*50)*4,1) ne "\x00"?"1":"0";
}
}

if($min_top>$min_y){$min_top=$min_y;}
$font[$charcode]=[$width,$height,$min_y,$pix];

}

save_font();

########
=pod

  # - top point               -		 	   \
			      |		            | 
			      > min_top		    > min_y
  #point of most top pixel    |			    |
						    |
  #point of average top pixel                      -




  ###### - base line

  # some bottom point
=cut
###########################################################

sub save_font{

if(@font){

my @newfont=();

#$text="ABCDE";

# make space symbol
$font[32]=[@{$font[48]}];
$font[32][3]="";

$topline=$font[48][2]-$min_top;
$baseline=-$font[48][1]-$topline;
$fontwidth=$font[48][0];
if(!defined $font[48][2]){
$baseline=-2;
$fontwidth=1;
}

my $ch;
for($ch=0;$ch<128;$ch++){
my($width,$height,$min_y,$pix)=@{$font[$ch]};
if($width==0 || $height==0 || $pix eq "0"){next;}
$newfont[$ch]=$font[$ch];
$newfont[$ch][2]-=$min_top;
}
$fonts[$fontwidth]=[$baseline,\@newfont];

}
$min_top=0xFFFF;
@font=();

}

#if($min_x!=3000 && $min_y!=3000){#$pic->rectangle($pos_x+$min_x,$pos_y+$min_y,$pos_x+$max_x,$pos_y+$max_y,$red);}


$text="Ya ebal eti ebanie shrifty! $font_id";
$pos_y=0;
for($size=1;$size<20;$size++){
$pos_x=0;
$pos_y+=$size*2+5;
($baseline,$font)=@{$fonts[$size]};
@font=@{$font};

#print Dumper(\@font);

foreach(split(//,$text)){
my $charcode=ord($_);
if(!defined $font[$charcode]){next;}
my($width,$height,$min_y,$pix)=@{$font[$charcode]};
print "Draw font $size, charcode $_ ".$charcode."\t at $pos_x x $pos_y @bits, $min_x,$min_y,$max_x,$max_y, min_y:$min_y / $min_top, base $baseline, $width x $height\n";
$offset_y=5+$size+$baseline+($min_y);

for($w=0;$w<$height;$w++){
for($q=0;$q<$width;$q++){
$pic->setPixel($pos_x+$q,$pos_y+$w+$offset_y,substr($pix,$q+$w*$width,1) eq "1"?$white:$black);
}
}

$pos_x+=$width+1;
if($pos_x>500){$pos_x=0;$pos_y+=16;}
}
}


open(oo,">out.png");
binmode(oo);
print oo $pic->png(9);
close(oo);
