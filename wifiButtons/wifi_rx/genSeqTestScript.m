function genSeqTestScript()

fid = fopen('testUdpCommands.sh', 'w');

fprintf(fid, '#!/bin/bash\n\n');

% Wrong sequence (basically just a random sequence)
fprintf(fid, '# wrong (random) sequence\n');
N = 50;
Nbuttons = 8;

for n=1:N
    btnState = round(rand(1));
    btnIndex = floor(Nbuttons*rand(1));
    fprintf(fid, 'echo "BTN%u%s" > /dev/udp/192.168.0.1/16\n', btnState, char(btnIndex+'A'));
end
fprintf(fid, '\n');

fprintf(fid, '# correct true sequence\n');
correctSequence = 'ADCBCABD';
for n=1:length(correctSequence)
    fprintf(fid, 'echo "BTN1%s" > /dev/udp/192.168.0.1/16\n', correctSequence(n));
    fprintf(fid, 'echo "BTN0%s" > /dev/udp/192.168.0.1/16\n', correctSequence(n));
end
fprintf(fid, '\n');

% even more random stuff
fprintf(fid, '# (even more) wrong (random) sequence\n');
for n=1:N
    btnState = round(rand(1));
    btnIndex = floor(Nbuttons*rand(1));
    fprintf(fid, 'echo "BTN%u%s" > /dev/udp/192.168.0.1/16\n', btnState, char(btnIndex+'A'));
end
fprintf(fid, '\n');

fclose(fid);
