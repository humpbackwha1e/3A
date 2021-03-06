trainImages = loadMNISTImages('train-images.idx3-ubyte');
trainLabels = loadMNISTLabels('train-labels.idx1-ubyte');
validImages = loadMNISTImages('t10k-images.idx3-ubyte');
validLabels = loadMNISTLabels('t10k-labels.idx1-ubyte');
blr=zeros(1,10);
trainImagesSize=size(trainImages);
trainLabelsSize=size(trainLabels);
validImagesSize=size(validImages);
validLabelsSize=size(validLabels);
Wlr=rand(trainImagesSize(1),10);
sumExpa=0;  
dEx=zeros(trainImagesSize(1),10);
t=zeros(trainImagesSize(2),10);
numberOfIterations=30;
eta=0.1;
count=0;

for i=1:trainLabelsSize(1)
  t(i,trainLabels(i)+1)=1;
end

dExPrev=0;
for k2=1:numberOfIterations
    for i2=1:trainImagesSize(2)
          a=Wlr.'*trainImages(:,i2)+blr.';
          a=a/max(a);
          y=exp(a)/sum(exp(a));
          
          ynew=zeros(size(y));
          ynew(find(y == max(y), 1))=1;
          y=ynew;
          
          dEx=trainImages(:,i2)*(y-t(i2,:).').';
          Wlr=Wlr-eta*dEx;
%          eta=eta*0.9999;
    end
    eta=eta/2;
    %BOLD DIVER METHOD
    %if(dExPrev<=dEx)
    %    eta=eta/2;
    %else
    %    eta=eta*1.05;
    %end
    %dExPrev=dEx;
end


for i2=1:validImagesSize(2)
  a=Wlr.'*validImages(:,i2)+blr.';
  a=a/max(a);
  y=exp(a)/sum(exp(a));
  if(find(y == max(y), 1)==validLabels(i2)+1)
    count=count+1;
  end
end
count/validImagesSize(2)
save('proj3.mat','Wlr','blr');