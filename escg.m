function [embed,runtime] = escg(W, options)
n = size(W, 1);
d = options.d;
k = options.k;

% if options does not contain field embed, it means this is the first
% running of escg module, R is then generated randomly
% or R will be generated depending on options.embed
runtime = zeros(1,5);
tic
if isfield(options,'embed')
    R = sparse(2*k, n);
    blacklist = [];
    for i =  1:k
        m = mean(options.embed(:,i));          
        ind = find(options.embed(:,i)>=m);
        if length(ind) == n || isempty(ind)
            blacklist = [blacklist i*2-1 i * 2];
        end
        R(i*2-1, ind) = ones(1, length(ind));
        ind = find(options.embed(:,i)<m);      
        R(i * 2, ind) = ones(1, length(ind));
    end
    R(blacklist, :) = [];
else
    [i,j,s] = find(W);
    s = s ./ max(s);
    y= -log(s) + 10e-10;

    M = sparse(i, j, y, n, n);
    ind = randperm(n);
    D = dijkstra(M, ind(1:d));
    [~, i] = min(D);
    R = sparse(i, 1:n, 1, d, n); 
    clear i j s D ind y
end
runtime(1) = toc;

% obsorb R into W
tic
W_hat = sparse(R*W);

clear R
runtime(2) = toc;
tic

tmp = bsxfun(@times, W_hat, 1 ./ sqrt(sum(W_hat, 1)) );
W_hat_ = 1 ./ sqrt(sum(W_hat, 1))';

Z = bsxfun(@times, tmp, 1 ./ sqrt(sum(W_hat, 2)) );
Z(isnan(Z))=0;
clear tmp
clear W_hat


runtime(3) = toc;


% compute X
tic
[eigvec, eigval] = eigs(Z*Z',k);

embed = diag(1 ./ sqrt(diag(eigval))) * eigvec' * Z;
clear Z
runtime (4) = toc;

% compute U
tic
embed = embed';
embed = bsxfun( @times, embed, W_hat_);  
runtime(5) = toc;

