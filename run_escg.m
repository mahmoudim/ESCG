function [] = run_escg(d ,k , maxIter ,Replicates , nit)
if ~isnumeric(d)
    d = str2num(d);
end
if ~isnumeric(k)
    k = str2num(k);
end
if ~isnumeric(maxIter)
    maxIter = str2num(maxIter);
end
if ~isnumeric(Replicates)
    Replicates = str2num(Replicates);
end
if ~isnumeric(nit)
    nit = str2num(nit);
end
clearvars -except d k  maxIter Replicates nit
options.d = d;
% k: number of clusters
options.k = k;

load SymGraph.g
W=spconvert(SymGraph);

% W: adjacency matrix
embed = escg(W, options);
for num=2:nit
    options.embed=embed;
    embed = escg(W, options);
end
% run kmeans to get preditions
pred = litekmeans(embed, options.k, 'MaxIter', maxIter, 'Replicates', Replicates);

fileID = fopen('preds.names','w');
for col=1:size(W,1)
    fprintf(fileID,'%d %d\n',col,pred(col));
end
fclose(fileID);

CommunitiesC=1;
Communities=zeros(size(W,1),1);
for i = 1:options.k
    cluster=find(pred==i);
    SubG=W(cluster,cluster);
    [S,C]=graphconncomp(SubG);
    for j = 1:C
        com=cluster(find(C==1));
        if(size(com,1)>1)
            Communities(com)=CommunitiesC;
            CommunitiesC=CommunitiesC+1;
        end
    end
end

fileID = fopen('communities.names','w');
for col=1:size(W,1)
    fprintf(fileID,'%d %d\n',col,Communities(col));
end
fclose(fileID);

