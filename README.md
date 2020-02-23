# VideoClassification_TransferLearning

for train
 python train.py --dataset data --model model/activity.model --label-bin model/lb.pickle --epochs 80
 
 
for use 
 python predict_video.py --model model/activity_gpu.model --label-bin model/lb.pickle --input example_clips/cyclone_movie.mp4 --output output/cyclon_movie_output.avi --size 10
 
 
