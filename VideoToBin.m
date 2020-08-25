clc;
clear;

% 定义常量
VIDEO_NAME = 'BadApple.mp4';

IMAGE_WIDTH = 128;
IMAGE_HEIGHT = 64;
PATH = './images/';
POSTFIX = '.bmp';

numOfImages = 9480; % video.NumberOfFrames

% % 处理视频
% video = VideoReader(VIDEO_NAME);
% numOfImages = video.NumberOfFrames; % 9480
% for i = 1 : numOfImages
%     frame = read(video, i);
%     frame = imresize(frame, [IMAGE_HEIGHT IMAGE_WIDTH]); % 调整大小为128x64
%     name = sprintf('%04d', i - 1);
%     path = [PATH, name, POSTFIX];
%     imwrite(frame, path); % 保存帧为BMP图片
% end


% 读取并处理图片
imgs = zeros(numOfImages, IMAGE_WIDTH * IMAGE_HEIGHT);
for i = 1 : numOfImages
    A = logical([]);
    name = sprintf('%04d', i - 1);
    path = [PATH, name, POSTFIX];
    img = imread(path);
    bw = imbinarize(img); % 和im2bw会有一点点不同
    for j = 1 : 8
        for k = 1 : 128
            B = bw(1 + 8 * (j - 1) : 8 + 8 * (j - 1), k)';
            A = [A, B];
        end
    end
    imgs(i, :) = A;
end

% 编码
bin = uint8(zeros(numOfImages, IMAGE_WIDTH * IMAGE_HEIGHT / 8));
for i = 1 : numOfImages
    for j = 1 : IMAGE_WIDTH * IMAGE_HEIGHT / 8
        for k = 0 : 7
            bin(i, j) = bin(i, j) + imgs(i, k + 1 + (j - 1) * 8) * 2^k;
        end
    end
end

% 保存为bin文件
bin = bin';
fp = fopen('BadApple.bin', 'wb');
fwrite(fp, bin, 'uint8');
fclose(fp);
