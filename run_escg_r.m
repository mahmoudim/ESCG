clear options
% d: number of supernodes
options.d = 10;
% k: number of clusters
options.k = 4;
% t: number of iterations for ESCG-R
niter = 10;

embed = escg(W, options);
    
for i=2:niter
    options.embed = embed;
    embed = escg(W, options);
end

pred = litekmeans(embed, options.k, 'MaxIter', 200, 'Replicates', 20);