/* In The Name of Allah */

#include "soundserver_parent_main.h"

#ifdef UMMAH
const char *json_file_param = "name";
const char *json_safespaceflag_param = "safe_space_mode";
#else
const char *json_file_param = "sound";
#endif /* UMMAH */

const char *failsound = "std_sounds/jungle2.wav";
// needs slash at end
const char *sounds_dir = "std_sounds/";

#ifdef UMMAH
const char *sounds[] =
{
        "akhi_abligh_hona_alasshab.ogg",
        "habat_ka_sayf.ogg",
        "srna.ogg",
        "ziegenficker.ogg",
        "hebbit_kerrih.ogg",
        "qum.ogg",
        "salil_sawarim.ogg",
        "saraya_dawlati.ogg"
};
#endif

void play_sound(const char *filename)
{
        libvlc_play(filename);
}

int msg_parse_proc_main()
{
        // switch parameter that points to the
        // variable where the suggested filename
        // is stored before setting together the path
        // to avoid too many IFDEFs
        const char *fn_parm = NULL;

        // sound playback variables
        FILE *audiofile = NULL;
        char *filename = NULL;

#ifdef UMMAH
        // lookup3 variable
        uint32_t soundindex = 0;
        // safe space flag
        char safe_space_mode = 0;
#endif

        char buf[FNBUF_S+1];
        buf[FNBUF_S]='\0';

        // rabbitmq variables
        amqp_connection_state_t conn = NULL;
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        // json-c variables
        json_object *json_msg = NULL;
        json_object *extracted_field = NULL;
        json_object *inner_field = NULL;
        char *msg_cstr = NULL;
        const char *json_filename = NULL;


        // rabbitmq setup
        rmq_setup(&conn);
        
        // end setup rmq



        // TEST SETUP TO READ MESSAGES, NOT FINAL
        while (1)
        {
                amqp_maybe_release_buffers(conn);
                
                // TODO: is envelope allocated even if error below?
                res = amqp_consume_message(conn, &envelope, NULL, 0);
                
                if(AMQP_RESPONSE_NORMAL != res.reply_type)
                {
                        // TODO: log error
                        amqp_destroy_envelope(&envelope);
                        continue;
                }

                // read from message into json object
                msg_cstr = amqp_bytes_to_str(&(envelope.message.body));
                json_msg = json_tokener_parse(msg_cstr);
                free(msg_cstr);
                msg_cstr = NULL;
                
                // search for a filename field
                if(json_object_object_get_ex(json_msg, "cmd", &extracted_field))
                {
#ifdef UMMAH
                        if(json_object_object_get_ex(extracted_field, json_safespaceflag_param, &inner_field) &&
                           json_object_get_boolean(inner_field))
                        {
                            safe_space_mode = 1;
                        }
                        else
#endif
                        if(json_object_object_get_ex(extracted_field, json_file_param, &inner_field) &&
                           json_object_get_string_len(inner_field) > 0)
                        {
                                // memory is somehow managed by json library, so no free()ing
                                json_filename = json_object_get_string(inner_field);
                                strncpy(buf, json_filename, FNBUF_S);
                                assert('\0' == buf[FNBUF_S]);
                        }
                }

                // make file name operations on correct file name
                // depending on whether UMMAH mode is activated
#ifdef UMMAH
                if(safe_space_mode == 0)
                {
                    // hash buffer and choose soundfile from hash
                    soundindex = hashlittle(buf, strlen(buf), 0) % sizeof(*sounds);
                    fn_parm = sounds[soundindex];
                }
#else /* NOT UMMAH */
                fn_parm = buf;
#endif

                // check whether filename has no directories inside
                if(
#ifdef UMMAH
                   // only play sounds if no safe space mode active
                   safe_space_mode == 0 &&
#endif /* UMMAH */
                   NULL == strchr(fn_parm, '/') &&
                   NULL == strchr(fn_parm, '\\'))
                {
                        // add directory to filename
                        filename = (char *) malloc(strlen(fn_parm)+strlen(sounds_dir)+1);

                        if(filename == NULL)
                        {
                                fprintf(stderr, "could not allocate memory for filepath string");
                                exit(11);
                        }

                        strncpy(filename, sounds_dir, strlen(sounds_dir));
                        strncpy(filename+strlen(sounds_dir), fn_parm, strlen(fn_parm));
                        filename[strlen(fn_parm)+strlen(sounds_dir)] = '\0';

                        // testing
                        //printf("sounddir=%s\nfile parameter=%s\nfilename=%s\n",sounds_dir,fn_parm,filename);
                        
                        // check whether file exists (and thus filename is valid)
                        // by trying to read-open it
                        audiofile = fopen(filename, "r");

                        if(NULL != audiofile)
                        {
                                // i hope the compiler doesn't optimize this check
                                // for existing file away
                                fclose(audiofile);

                                // actually play sound
                                play_sound(filename);
                         }
                        else
                        {
                                play_sound(failsound);
                        }
                                        
                        free(filename);
                        filename = NULL;
                }

                
                // free the object
                json_object_put(json_msg);
                
                amqp_destroy_envelope(&envelope);
        }


        // free rmq resources
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        
        return 0;
}

