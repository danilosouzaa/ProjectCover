use strict;
use warnings;
use 5.010;
use POSIX qw(strftime);

sub read_lines {
    my ($file) = @_;
 
    open my $in, "<:encoding(utf8)", $file or die "$file: $!";
    local $/ = undef;
    my $content = <$in>;
    close $in;
    return split /\n/, $content;
}

my @instances = read_lines("../inst/InstancesCollectionOK.txt");
my @p1 =("1");
my @p2 =("1");
my @p3 =("1");
my @p4 =("1");
my $count = 1;
my $flag = 0;
my $data = strftime "%F", localtime;
foreach my $pn1 (@p1){
	foreach my $pn2 (@p2){
		foreach my $pn3 (@p3){
			foreach my $pn4 (@p4){
				if( $pn2 eq "0" & $pn3 eq "0" & $pn4 eq "0" & ($pn1 eq "0.5" | $pn1 eq "1") ){

				}else{
					foreach my $record (@instances) {
						if($record ne "\n"){
							if($flag==0){
								system("mkdir ../saida/exp-$data-$count");
								$flag=1;
							}
							open(FIN,">>../saida/exp-$data-$count/Resultado-$record.txt");
							print FIN ("exp. $record $pn1 $pn2 $pn3 $pn4 \n");
							close(FIN);	
							system("./separationCuts SelectedCollection/$record 1000 12 0 0 200 60 50 $pn1 $pn2 $pn3 $pn4 >>../saida/exp-$data-$count/Resultado-$record.txt 2>&1");	
						}
					}
					$count++;
					$flag = 0;
				}
			}
		}
	}
   
}

exit;
