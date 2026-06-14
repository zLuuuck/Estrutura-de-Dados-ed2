#ifndef AUDIO_H
#define AUDIO_H

/*
 * Wrapper sobre miniaudio.h.
 * Nenhum outro modulo deve incluir miniaudio.h diretamente.
 *
 * Fluxo de uso esperado:
 *   audio_init()
 *   audio_load(caminho)  ->  audio_play()
 *   ... controles ...
 *   audio_stop()         ->  audio_load(outro) ou audio_shutdown()
 *   audio_shutdown()
 */

/* Inicializa o engine de audio; retorna 1 em sucesso, 0 em falha */
int audio_init(void);

/* Encerra o engine e libera todos os recursos */
void audio_shutdown(void);

/*
 * Carrega um arquivo de audio (MP3, WAV, FLAC, etc.).
 * Se ja houver um arquivo carregado, descarrega antes.
 * Retorna 1 em sucesso, 0 em falha.
 */
int audio_load(const char *caminho);

/* Inicia a reproducao desde o inicio */
void audio_play(void);

/* Pausa a reproducao sem descarregar */
void audio_pause(void);

/* Retoma apos pause */
void audio_resume(void);

/*
 * Avanca ou recua a posicao de reproducao em `segundos`.
 * Valores positivos avancam, negativos recuam.
 * Nao ultrapassa os limites (0 e duracao total).
 */
void audio_seek_relative(float segundos);

/* Retorna a posicao atual em segundos (0.0 se nada carregado) */
float audio_get_position_seconds(void);

/* Retorna a duracao total em segundos (0.0 se nada carregado) */
float audio_get_duration_seconds(void);

/* Retorna 1 se estiver tocando ativamente */
int audio_is_playing(void);

/* Retorna 1 se o arquivo chegou ao fim */
int audio_is_at_end(void);

/* Para a reproducao e descarrega o arquivo atual */
void audio_stop(void);

#endif /* AUDIO_H */
