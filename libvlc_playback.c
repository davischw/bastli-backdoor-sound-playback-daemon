#include "libvlc_playback.h"
#include <vlc/vlc.h>


libvlc_instance_t *inst;
libvlc_media_player_t *mp;

int libvlc_init(void){
	inst = libvlc_new(0,NULL);
	if(inst==NULL){
		return -1;
	}
	mp = libvlc_media_player_new(inst);
	if(mp==NULL){
		libvlc_release(inst);
		return -1;
	}
	return 0;
}
int libvlc_deinit(void){
	libvlc_media_player_release(mp);
	libvlc_release(inst);
	return 0;
}
int libvlc_play(const char* file){
        if(1 == libvlc_media_player_is_playing(mp))
        {
            return 0;
        }
	libvlc_media_t *m=libvlc_media_new_path(inst, file);
	libvlc_media_player_set_media(mp,m);
	libvlc_media_player_play(mp);
	libvlc_media_release(m);
	return 0;
}
