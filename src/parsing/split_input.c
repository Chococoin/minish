/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: siellage <siellage@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 16:42:11 by glugo-mu          #+#    #+#             */
/*   Updated: 2026/01/30 16:18:02 by siellage         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ensure_capacity(t_split_ctx *ctx)
{
	char	**new_args;
	size_t	new_capacity;
	size_t	i;

	if (ctx->count < ctx->capacity)
		return (1);
	new_capacity = ctx->capacity * 2;
	new_args = malloc(sizeof(char *) * (new_capacity + 1));
	if (!new_args)
		return (0);
	i = 0;
	while (i < ctx->count)
	{
		new_args[i] = ctx->tokens[i];
		i++;
	}
	free(ctx->tokens);
	ctx->tokens = new_args;
	ctx->capacity = new_capacity;
	return (1);
}

// static int	add_token(t_split_ctx *ctx, char *input, size_t *idx)
// {
// 	size_t	i;

// 	if (!ensure_capacity(ctx))
// 		return (0);
// 	ctx->tokens[ctx->count++] = &input[*idx];
// 	i = *idx;
// 	while (input[i] && input[i] != ' ' && input[i] != '\t')
// 	{
// 		if (input[i] == '\'' || input[i] == '\"')
// 		{
// 			if (!process_quote(input, &i, ctx))
// 				return (0);
// 		}
// 		if (input[i])
// 			i++;
// 	}
// 	if (input[i])
// 		input[i++] = '\0';
// 	*idx = i;
// 	return (1);
// }

// | > >> < <<
// "cat |cat | ls|cat"

int	ft_isspace(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}
int	ft_isspecialchar(char c)
{
	return (c == '|' || c == '>' || c == '<');
}

static int	add_token(t_split_ctx *ctx, char *input, int start, int end)
{
	size_t	len;
	char	*joined;
	char	*sub;

	ctx->end = end;
	ctx->start = ctx->end + 1;
	if (!ensure_capacity(ctx))
		return (0);
	len = end - start + 1;
	if (start > 0 && (!ft_isspace(input[start - 1])
			&& !ft_isspecialchar(input[start - 1])))
	{
		sub = ft_substr(input, start, len);
		joined = ft_strjoin(ctx->tokens[ctx->count - 1], sub);
		free(ctx->tokens[ctx->count - 1]);
		free(sub);
		ctx->tokens[ctx->count - 1] = joined;
	}
	else
		ctx->tokens[ctx->count] = ft_substr(input, start, len);
	if (!ctx->tokens[ctx->count])
		return (0);
	ctx->count++;
	return (1);
}

int	get_quote_closing_idx(t_split_ctx *ctx, char *input, int *start_idx)
{
	char	quote;
	int		i;

	quote = input[*start_idx];
	i = *start_idx + 1;
	while (input[i])
	{
		if (input[i] == quote)
		{
			*start_idx = i;
			return (i);
		}
		i++;
	}
	ctx->quote_error = quote;
	return (-1);
}



int	is_generic_word(char current, char next)
{
	return (((!ft_isspace(current) && !ft_isspecialchar(current))
			&& (ft_isspace(next)
				|| next == '\0'
				|| ft_isspecialchar(next)
				|| (current == '\'' && next == '"')
			)));
}

static int	fill_tokens(t_split_ctx *ctx, char *input)
{
	int	i;

	i = -1;
	while (input[++i])
	{
		if (input[i] == '\'' || input[i] == '\"')
		{
			ctx->end = get_quote_closing_idx(ctx, input, &i);
			if (ctx->end == -1 || !add_token(ctx, input, ctx->start, ctx->end))
				return (0);
		}
		if (ft_isspace(input[i]) && !ft_isspace(input[i + 1]))
			ctx->start = i + 1;
		if (is_generic_word(input[i], input[i + 1]))
		{
			if (!add_token(ctx, input, ctx->start, i))
				return (0);
		}
		if (ft_isspecialchar(input[i]) && input[i] != input[i + 1])
		{
			if (!add_token(ctx, input, ctx->start, i))
				return (0);
		}
	}
	return (1);
}

// static int	fill_tokens(t_split_ctx *ctx, char *input)
// {
// 	size_t	i;

// 	i = 0;
// 	while (input[i])
// 	{
// 		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
// 			i++;
// 		if (!input[i])
// 			break ;
// 		if (!add_token(ctx, input, &i))
// 			return (0);
// 	}
// 	return (1);
// }

char	**split_input(char *input)
{
	t_split_ctx	ctx;

	if (!input)
		return (NULL);
	ctx.capacity = 8;
	ctx.quote_error = 0;
	ctx.start = 0;
	ctx.end = 0;
	ctx.tokens = malloc(sizeof(char *) * (ctx.capacity + 1));
	if (!ctx.tokens)
		return (NULL);
	ctx.count = 0;
	if (!fill_tokens(&ctx, input))
	{
		if (ctx.quote_error)
			printf("minishell: unexpected EOF while looking for "
				"matching `%c'\n", ctx.quote_error);
		free(ctx.tokens);
		return (NULL);
	}
	ctx.tokens[ctx.count] = NULL;
	return (ctx.tokens);
}
