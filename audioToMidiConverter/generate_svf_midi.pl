print "0, 0, Header, 1, 1, 480\n";
print "1, 0, Start_track\n";
print "1, 0, Tempo, 500000\n";
$TIMER = 0;
while ($a = <>) {
    $note = $a + 30;
    print "1, $TIMER, Note_on_c, 1, $note, 127\n";
    $TIMER += 127;
    print "1, $TIMER, Note_off_c, 1, $note, 0\n";
}
$TIMER += 10;
print "1, $TIMER, End_track\n";
print "0, 0, End_of_file\n";