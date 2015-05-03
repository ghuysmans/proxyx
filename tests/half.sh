#This Perl one-liner was taken from http://www.perlmonks.org/?node_id=1111789.
perl -MHTTP::Daemon -e '$d = HTTP::Daemon->new(LocalPort => 2043) or  die $!; while ($c = $d->accept) { while ($r = $c->get_request) {     $c->send_file_response("tests/half".$r->url->path) } }' &

sleep 0.5
cat tests/half/{a,b,c}.exp >tests/half.exp
for x in tests/half/{a,b,c}; do ../../proxyx -d <$x; done

kill $!
